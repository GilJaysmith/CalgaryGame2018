#include "Engine/Pch.h"

#include "Engine/Input/Gamepad.h"
#include "Engine/Input/Input.h"
#include "Engine/Vehicles/VehicleInputHandler.h"


VehicleInputHandler::VehicleInputHandler(int player_idx)
	: m_PlayerIdx(player_idx)
	, m_Acceleration(0.0f)
	, m_Brake(0.0f)
	, m_Handbrake(false)
	, m_Steer(0.0f)
	, m_Jump(false)
	, m_ResetOrientation(false)
{
	Input::RegisterInputHandler(this);
}

VehicleInputHandler::~VehicleInputHandler()
{
	Input::UnregisterInputHandler(this);
}

void VehicleInputHandler::Update()
{
	// Keyboard.
	m_Acceleration = Input::IsPressed(GLFW_KEY_I) ? 1.0f : 0.0f;
	m_Brake = Input::IsPressed(GLFW_KEY_K) ? 1.0f : 0.0f;
	m_Handbrake = Input::IsPressed(GLFW_KEY_M);
	m_Steer = Input::IsPressed(GLFW_KEY_J) ? 1.0f : Input::IsPressed(GLFW_KEY_L) ? -1.0f : 0.0f;
	m_Jump = Input::IsPressed(GLFW_KEY_SPACE);
	m_ResetOrientation = Input::IsPressed(GLFW_KEY_R);

	// Gamepad.
	GamepadHandler* gamepad = GamepadManager::GamepadForPlayer(m_PlayerIdx);
	if (gamepad)
	{
		m_Acceleration = (gamepad->Axis(GamepadAxis::RIGHT_TRIGGER) + 1.0f) / 2.0f;
		m_Brake = (gamepad->Axis(GamepadAxis::LEFT_TRIGGER) + 1.0f) / 2.0f;
		m_Handbrake = gamepad->ButtonPressed(GamepadButton::X);
		m_Steer = -gamepad->Axis(GamepadAxis::LEFT_STICK_HORIZ);
		m_Jump = gamepad->ButtonPressed(GamepadButton::A);
	}
}
