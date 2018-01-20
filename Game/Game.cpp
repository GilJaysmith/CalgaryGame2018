// Game.cpp : Defines the entry point for the application.

#include "Pch.h"
#include "Game.h"

#include "../Engine/Memory/Memory.h"
#include "../Engine/Physics/Physics.h"
#include "../Engine/Rendering/Renderer.h"
#include "../Engine/Rendering/ShaderManager.h"

#define GLEW_STATIC
#include "../sdks/glew/include/GL/glew.h"

#include "../sdks/glfw/include/GLFW/glfw3.h"
#include "../sdks/soil/include/SOIL.h"

#include "../sdks/glm/glm.hpp"
#include "../sdks/glm/gtc/matrix_transform.hpp"
#include "../sdks/glm/gtc/type_ptr.hpp"

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
	ShaderManager::Initialize();

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
	glfwSwapInterval(1);

	glewExperimental = GL_TRUE;
	glewInit();

	unsigned int vertex_shader = ShaderManager::LoadShader("TestVertexShader", GL_VERTEX_SHADER);
	unsigned int fragment_shader = ShaderManager::LoadShader("TestFragmentShader", GL_FRAGMENT_SHADER);

	unsigned int shader_program = ShaderManager::MakeProgram({ vertex_shader, fragment_shader }, { {0, "outColor"} });

	glUseProgram(shader_program);

	GLfloat vertices[] = {
		//  Position      Color             Texcoords
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shader_program, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	GLint texAttrib = glGetAttribLocation(shader_program, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	// Load texture
	GLuint tex;
	glGenTextures(1, &tex);

	int width, height;
	unsigned char* image = SOIL_load_image("Data/Textures/SquarePanda.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	GLint uniTrans = glGetUniformLocation(shader_program, "trans");

	GLint uniTime = glGetUniformLocation(shader_program, "time");

	float time = 0.0f;
	int rot = 0;

	bool update_time = false;
	bool update_rotation = false;
	int r_state = GLFW_RELEASE;
	int t_state = GLFW_RELEASE;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (update_time)
		{
			time += 0.01f;
			glUniform1f(uniTime, time);
		}

		if (update_rotation)
		{
			rot = (rot + 1) % 360;
		}
		glm::mat4 trans;
		trans = glm::rotate(trans, glm::radians(float(rot)), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (glfwGetKey(window, GLFW_KEY_T) != t_state)
		{
			t_state = glfwGetKey(window, GLFW_KEY_T);
			if (t_state == GLFW_PRESS)
			{
				update_time = !update_time;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_R) != r_state)
		{
			r_state = glfwGetKey(window, GLFW_KEY_R);
			if (r_state == GLFW_PRESS)
			{
				update_rotation = !update_rotation;
			}
		}
	}

	glfwTerminate();

	ShaderManager::Terminate();
	Rendering::Terminate();
	Physics::Terminate();

	return 0;
}
