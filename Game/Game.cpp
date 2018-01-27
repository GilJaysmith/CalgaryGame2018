// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/GameStateManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

#include "Game/Game.h"
#include "Game/GameStates/TestGameState.h"
#include "Game/GIS/GISManager.h"

#include "sdks/assimp/include/Importer.hpp"
#include "sdks/assimp/include/scene.h"
#include "sdks/assimp/include/postprocess.h"

#include "sdks/fmod/include/fmod.hpp"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Memory::RegisterAllocator(MemoryPool::System, Memory::CreateFreeStoreAllocator());
	Memory::RegisterAllocator(MemoryPool::Physics, Memory::CreateFreeStoreAllocator());
	Memory::RegisterAllocator(MemoryPool::Rendering, Memory::CreateFreeStoreAllocator());

	Physics::Initialize();
	Renderer::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	MeshManager::Initialize();
	Input::Initialize();
	GameStateManager::Initialize();
	EntityManager::Initialize();
	GIS::Initialize();

	GIS::LoadCity("vancouver");

	Assimp::Importer importer;

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	FMOD::System* fmod = nullptr;
	FMOD::System_Create(&fmod);
	fmod->init(512, FMOD_INIT_NORMAL, 0);

	ComponentRegistry::RegisterComponentType("render", RenderComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("physics", PhysicsComponent::CreateComponent);

	int x = 800;
	int y = 600;
	bool full_screen = false;
	Renderer::CreateWindow(x, y, full_screen);

	TestGameState* test_game_state = MemNew(MemoryPool::System, TestGameState);
	GameStateManager::RequestGameState(test_game_state);

	Time last_frame_time = Time::fromSeconds(1.0f / 60.0f);

	bool update_physics = false;

	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result = fmod->createSound("Data\\Audio\\a2002011001-e02.wav", FMOD_CREATESAMPLE, nullptr, &sound);

	result = fmod->playSound(sound, nullptr, false, nullptr);

	while (1)
	{
		fmod->update();

		GLFWwindow* window = Renderer::GetWindow();
		CheckGLError();

		Input::Update(window);
		CheckGLError();

		if (Input::GetKeyEvent(GLFW_KEY_P) == Input::PRESSED)
		{
			update_physics = !update_physics;
		}

		if (!GameStateManager::Update(last_frame_time))
		{
			break;
		}

		if (update_physics)
		{
			Physics::Simulate(last_frame_time);
		}
		
		EntityManager::Update(last_frame_time);
		EntityManager::DestroyPendingEntities();

		glfwSwapBuffers(window);
		Renderer::ClearWindow();
		Renderer::RenderScene();

		glfwPollEvents();

		if (Input::GetKeyEvent(GLFW_KEY_ESCAPE) == Input::PRESSED)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (glfwWindowShouldClose(window))
		{
			GameStateManager::RequestGameState(nullptr);
		}
	}

	CoUninitialize();

	GIS::Terminate();
	EntityManager::Terminate();
	GameStateManager::Terminate();
	Input::Terminate();
	MeshManager::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();

	return 0;
}
