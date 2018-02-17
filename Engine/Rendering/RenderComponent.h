#pragma once

#include "Engine/Entities/Component.h"


class Entity;
class Mesh;
class Renderable;

class RenderComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~RenderComponent();

	void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;

protected:
	RenderComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) override;

	Renderable* m_Renderable;
};
