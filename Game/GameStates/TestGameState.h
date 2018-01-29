#pragma once

#include "Engine/GameStates/GameState.h"

class DebugCamera;
class Entity;
class Time;

class TestGameState : public GameState
{
public:
	TestGameState();
	~TestGameState();

	virtual void OnEnter() override;
	virtual void OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	float m_Time;
	bool m_UpdateTime;

	DebugCamera* m_Camera;

	Entity* m_Entities[1000];
	Entity* m_GroundPlane;
};
