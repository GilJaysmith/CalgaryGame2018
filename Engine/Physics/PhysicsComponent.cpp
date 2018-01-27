#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

#include "PxPhysicsAPI.h"
#include "foundation/PxFoundation.h"

Component* PhysicsComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Rendering, PhysicsComponent)(owner, properties);
}

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::OnUpdate(const Time& elapsed_time)
{
	physx::PxMat44 matrix_transform(m_Actor->getGlobalPose());
	glm::mat4 new_world_transform;
	for(int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			new_world_transform[i][j] = matrix_transform[i][j];
		}
	}
	m_Entity->SetTransform(new_world_transform);
}

PhysicsComponent::PhysicsComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
	const glm::mat4& transform = owner->GetTransform();
	glm::vec4 position = transform[3];
	m_Actor = Physics::GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z)));
	physx::PxMaterial* material = Physics::GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	m_Actor->createShape(physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *material);
	Physics::GetScene()->addActor(*m_Actor);
}

void PhysicsComponent::OnSetActive(bool active)
{
	m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);
}