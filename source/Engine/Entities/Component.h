#pragma once

#include "Engine/Entities/UpdatePass.h"

class Entity;
class Message;
class Time;

class Component
{
public:
	Component(Entity* owner);
	virtual ~Component();

	virtual bool OnMessage(Message*) { return false; }
	virtual void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) {}
	virtual void OnTransformUpdated() {}

	void SetActive(bool active);

protected:

	virtual void OnSetActive(bool active) {}

	Entity* m_Entity;
	bool m_Active;
};
