// Game.cpp : Defines the entry point for the application.

#include "Pch.h"
#include "Game.h"

#include "../Engine/Memory/Memory.h"
#include "../Engine/Physics/Physics.h"
#include "../Engine/Rendering/Renderer.h"

#define GLEW_STATIC
#include "../sdks/glew/include/GL/glew.h"

#include "../sdks/glfw/include/GLFW/glfw3.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	Memory::RegisterAllocator(MemoryPool::System, Memory::CreateFreeStoreAllocator());
	Memory::RegisterAllocator(MemoryPool::Physics, Memory::CreateFreeStoreAllocator());

	Physics::Initialize();

	Rendering::Initialize();

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = nullptr;

	int x = 800;
	int y = 600;
	bool full_screen = false;

	if (full_screen)
	{
		window = glfwCreateWindow(x, y, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
	}
	else
	{
		window = glfwCreateWindow(x, y, "OpenGL", nullptr, nullptr); // Windowed
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	glfwTerminate();

	Physics::Terminate();

	return 0;
}
