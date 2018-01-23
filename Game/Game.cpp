// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include "Game/Game.h"
#include "Engine/GameStates/GameStateManager.h"
#include "Game/GameStates/TestGameState.h"


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
	Input::Initialize();
	GameStateManager::Initialize();

	int x = 800;
	int y = 600;
	bool full_screen = false;
	Renderer::CreateWindow(x, y, full_screen);

	TestGameState* test_game_state = MemNew(MemoryPool::System, TestGameState);
	GameStateManager::RequestGameState(test_game_state);

	Time last_frame_time = Time::fromSeconds(1.0f / 60.0f);

	while (1)
	{
		GLFWwindow* window = Renderer::GetWindow();
		Input::Update(window);

		Renderer::ClearWindow();
		if (!GameStateManager::Update(last_frame_time))
		{
			break;
		}

		Renderer::RenderScene();

		glfwSwapBuffers(window);
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

	Input::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();

	return 0;
}
