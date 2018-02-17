// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include <chrono>

#include "Engine/Audio/Audio.h"
#include "Engine/DebugPanels/DebugPanels.h"
#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/GameStateManager.h"
#include "Engine/GameStates/SplashScreenGameState.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"
#include "Engine/Vehicles/VehicleComponent.h"

#include "Game/Game.h"
#include "Game/GameStates/CityGameState.h"
#include "Game/GameStates/TestGameState.h"
#include "Game/GIS/GISManager.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Memory::Initialize();

	Memory::RegisterAllocator(MemoryPool::System, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Physics, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::Multithreaded);
	Memory::RegisterAllocator(MemoryPool::Rendering, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Entities, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Audio, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Vehicles, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);

	Audio::Initialize();
	Physics::Initialize();
	Renderer::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	ScreenSpaceRenderer::Initialize();
	DebugPanels::Initialize();
	MeshManager::Initialize();
	Input::Initialize();
	GameStateManager::Initialize();
	EntityManager::Initialize();
	GIS::Initialize();

	ComponentRegistry::RegisterComponentType("render", RenderComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("physics", PhysicsComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("vehicle", VehicleComponent::CreateComponent);

	SplashScreenGameState* splash_screen_game_state = MemNew(MemoryPool::System, SplashScreenGameState)("Intro1.png", Time::fromSeconds(5.0f));
	//GameStateManager::QueueGameState(splash_screen_game_state);
	//CityGameState* game_state = MemNew(MemoryPool::System, CityGameState);
	TestGameState* game_state = MemNew(MemoryPool::System, TestGameState);
	GameStateManager::QueueGameState(game_state);

	Time last_frame_time = Time::fromSeconds(1.0f / 60.0f);

	while (1)
	{
		auto frame_start_time = std::chrono::system_clock::now();

		GLFWwindow* window = Renderer::GetWindow();

		DebugPanels::Update(last_frame_time);

		Audio::Update(last_frame_time);

		Input::Update(window);

		if (!GameStateManager::Update(last_frame_time))
		{
			break;
		}

		EntityManager::Update(last_frame_time, UpdatePass::BeforePhysics);

		Physics::Simulate(last_frame_time);
		
		EntityManager::Update(last_frame_time, UpdatePass::AfterPhysics);

		EntityManager::DestroyPendingEntities();

		glfwSwapBuffers(window);
		Renderer::ClearWindow();

		Renderer::RenderScene();
		GIS::Render();
		ScreenSpaceRenderer::Render();
		DebugPanels::Render();

		glfwPollEvents();

		if (Input::GetKeyEvent(GLFW_KEY_ESCAPE) == Input::PRESSED)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (glfwWindowShouldClose(window))
		{
			GameStateManager::ForceQuit();
		}

		auto frame_end_time = std::chrono::system_clock::now();
		auto frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end_time - frame_start_time).count() / float(1000);
		last_frame_time = Time::fromSeconds(std::max(frame_time, 1.0f / 60.0f));
	}

	MemDelete(game_state);
	MemDelete(splash_screen_game_state);

	GIS::Terminate();
	EntityManager::Terminate();
	GameStateManager::Terminate();
	Input::Terminate();
	MeshManager::Terminate();
	DebugPanels::Terminate();
	ScreenSpaceRenderer::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();
	Audio::Terminate();
	Memory::Terminate();

	return 0;
}
