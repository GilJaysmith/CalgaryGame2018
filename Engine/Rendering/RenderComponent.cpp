#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"

Component* RenderComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Rendering, RenderComponent)(owner, properties);
}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::OnUpdate(const Time& elapsed_time)
{
	const glm::mat4& world_transform = m_Entity->GetTransform();
	m_Renderable->SetTransform(world_transform);
}

RenderComponent::RenderComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
	for (auto property : properties)
	{
		std::string property_name = property.first.as<std::string>();
		if (property_name == "mesh")
		{
			std::string mesh_name = property.second.as<std::string>();
			Mesh* mesh = MeshManager::LoadMeshFromFile(mesh_name);
			m_Renderable = MemNew(MemoryPool::Rendering, Renderable)(mesh);
			Renderer::RegisterRenderable(m_Renderable);
		}
	}
}

void RenderComponent::OnSetActive(bool active)
{
	if (m_Renderable)
	{
		m_Renderable->SetActive(active);
	}
}