// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include "Game/Game.h"


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

	int x = 800;
	int y = 600;
	bool full_screen = false;

	Renderer::CreateWindow(x, y, full_screen);

	unsigned int vertex_shader = ShaderManager::LoadShader("TestVertexShader", GL_VERTEX_SHADER);
	unsigned int fragment_shader = ShaderManager::LoadShader("TestFragmentShader", GL_FRAGMENT_SHADER);
	unsigned int shader_program = ShaderManager::MakeProgram({ vertex_shader, fragment_shader }, { {0, "outColor"} });

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	Mesh* mesh = MemNew(MemoryPool::Rendering, Mesh);
	mesh->CreateFromVertexArray(vertices, sizeof(vertices), shader_program);
	mesh->SetTexture(GL_TEXTURE0, TextureManager::LoadTexture("Data/Textures/Panda_Hugs.png"));

	Renderable* renderable = MemNew(MemoryPool::Rendering, Renderable)(mesh);
	Renderable* renderable2 = MemNew(MemoryPool::Rendering, Renderable)(mesh);

	Renderer::RegisterRenderable(renderable);
	Renderer::RegisterRenderable(renderable2);

	glUseProgram(shader_program);

	GLint uniTime = glGetUniformLocation(shader_program, "time");

	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
	GLint uniView = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	float time = 0.0f;
	int rot = 0;

	bool update_time = false;
	bool update_rotation = false;

	GLFWwindow* window = Renderer::GetWindow();

	while (!glfwWindowShouldClose(window))
	{
		Input::Update(window);

		Renderer::ClearWindow();

		if (update_time)
		{
			time += 0.01f;
			glUniform1f(uniTime, time);
		}

		if (update_rotation)
		{
			rot = (rot + 1) % 360;
		}

		glm::mat4 world_transform;
		world_transform = glm::rotate(world_transform, glm::radians(float(rot)), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 world_transform2;
		world_transform2 = glm::translate(world_transform2, glm::vec3(0.0f, 1.0f, 0.0f));
		world_transform2 = glm::rotate(world_transform2, glm::radians(float(-rot)), glm::vec3(0.0f, 0.0f, 1.0f));

		renderable->SetTransform(world_transform);
		renderable2->SetTransform(world_transform2);

		Renderer::RenderScene();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (Input::GetKeyEvent(GLFW_KEY_ESCAPE) == Input::PRESSED)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (Input::GetKeyEvent(GLFW_KEY_T) == Input::PRESSED)
		{
			update_time = !update_time;
		}

		if (Input::GetKeyEvent(GLFW_KEY_R) == Input::PRESSED)
		{
			update_rotation = !update_rotation;
		}
	}

	Input::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();

	return 0;
}
