#include "Engine/Pch.h"

#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/Entities/Component.h"
#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"

#include "yaml-cpp/yaml.h"

unsigned int Entity::s_NextId = 1;

Entity* Entity::CreateEntity(const std::string& entity_type, const glm::mat4& transform)
{
	std::string yaml_filename = "data/entities/" + entity_type + ".yaml";
	YAML::Node yaml = YAML::LoadFile(yaml_filename);

	Entity* entity = EntityManager::CreateEntity();
	entity->m_Type = entity_type;
	entity->SetTransform(transform);
	for (auto behaviour : yaml["behaviours"])
	{
		Component* new_component = ComponentRegistry::CreateComponent(entity, behaviour.first.as<std::string>(), behaviour.second);
		entity->AddComponent(new_component);
	}
	return entity;
}

Entity::Entity()
	: m_Id(s_NextId++)
{
}

Entity::~Entity()
{
	for (auto component : m_Components)
	{
		MemDelete(component);
	}
}

void Entity::AddComponent(Component * component)
{
	m_Components.push_back(component);
}

int Entity::OnMessage(Message * message)
{
	int components_responding_to_message = 0;

	for (auto component : m_Components)
	{
		if (component->OnMessage(message))
		{
			++components_responding_to_message;
		}
	}

	return components_responding_to_message;
}

void Entity::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	for (auto component : m_Components)
	{
		component->OnUpdate(elapsed_time, update_pass);
	}
}

void Entity::SetTransform(const glm::mat4& transform, bool update_components)
{
	m_Transform = transform;
	if (update_components)
	{
		for (auto component : m_Components)
		{
			component->OnTransformUpdated();
		}
	}
}

void Entity::AddTag(const std::string& tag)
{
	EntityManager::AddTag(this, tag);
}

void Entity::RemoveTag(const std::string& tag)
{
	EntityManager::RemoveTag(this, tag);
}

void Entity::SetActive(bool active)
{
	for (auto component : m_Components)
	{
		component->SetActive(active);
	}
}
