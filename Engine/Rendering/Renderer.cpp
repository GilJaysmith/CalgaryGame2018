#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/DebugPanels/DebugPanels.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"
#include "Engine/Rendering/Viewport.h"

#include <set>

namespace Renderer
{
	void GLFWErrorFunction(int error, const char* message)
	{
		std::stringstream s;
		s << "Error " << error << " (" << message << ")";
		Logging::Log("GLFW", s.str());
	}

	GLFWwindow* s_Window = nullptr;

	unsigned int s_Width;
	unsigned int s_Height;
	bool s_FullScreen;

	GLuint s_SkyboxShader;
	GLuint s_CubemapTexture;
	unsigned int s_skyboxVAO, s_skyboxVBO;

	std::map<std::string, Viewport> s_Viewports;

	void DestroyWindow()
	{
		if (s_Window)
		{
			glfwDestroyWindow(s_Window);
			s_Window = nullptr;
		}
		s_Viewports.clear();
	}

	GLuint s_Framebuffer;
	GLuint s_TextureColourBuffer;
	GLuint s_PostprocessShader;	
	GLuint s_QuadVAO;

	void CreateWindow(unsigned int width, unsigned int height, bool full_screen)
	{
		DestroyWindow();

		s_Width = width;
		s_Height = height;
		s_FullScreen = full_screen;

		if (full_screen)
		{
			s_Window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
		}
		else
		{
			s_Window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); // Windowed
		}

		glfwMakeContextCurrent(s_Window);
		glfwSwapInterval(1);
		glEnable(GL_DEPTH_TEST);

		glewExperimental = GL_TRUE;
		glewInit();

		Viewport default_viewport(0, 0, width, height);
		s_Viewports["default"] = default_viewport;

		// Set up a framebuffer.
		glGenFramebuffers(1, &s_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_Framebuffer);
		// generate texture
		glGenTextures(1, &s_TextureColourBuffer);
		glBindTexture(GL_TEXTURE_2D, s_TextureColourBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_TextureColourBuffer, 0);
		// Set up a renderbuffer
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		// attach it to currently bound framebuffer object
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		// check framebuffer is complete
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);	
		// restore default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		s_PostprocessShader = ShaderManager::LoadProgram("Postprocess");

		float quad_vertices[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};
		glGenVertexArrays(1, &s_QuadVAO);
		glBindVertexArray(s_QuadVAO);
		GLuint quadVBO;
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
		GLint posAttrib = glGetAttribLocation(s_PostprocessShader, "position");
		GLint texAttrib = glGetAttribLocation(s_PostprocessShader, "texcoord");
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(posAttrib);
		glEnableVertexAttribArray(texAttrib);
	}

	void Initialize()
	{
		glfwSetErrorCallback(GLFWErrorFunction);
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		int x = 800;
		int y = 600;
		bool full_screen = false;
		CreateWindow(x, y, full_screen);

		s_SkyboxShader = ShaderManager::LoadProgram("Skybox");
		CheckGLError();
		s_CubemapTexture = TextureManager::LoadCubemap("cubemap.png");
		CheckGLError();

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &s_skyboxVAO);
		glGenBuffers(1, &s_skyboxVBO);
		glBindVertexArray(s_skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		CheckGLError();
	}

	void Terminate()
	{
		DestroyWindow();
		glfwTerminate();
	}

	void GetWindowDimensions(unsigned int& width, unsigned int& height)
	{
		width = s_Width;
		height = s_Height;
	}

	GLFWwindow* GetWindow()
	{
		return s_Window;
	}

	void ClearWindow()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	Camera* s_ActiveCamera = nullptr;

	void SetActiveCamera(Camera* camera)
	{
		if (s_ActiveCamera)
		{
			s_ActiveCamera->SetActive(false);
		}
		s_ActiveCamera = camera;
		if (s_ActiveCamera)
		{
			s_ActiveCamera->SetActive(true);
		}
	}

	Camera* GetActiveCamera()
	{
		return s_ActiveCamera;
	}

	std::set<Renderable*> m_RenderablesInScene;

	unsigned int s_PeakVertsInScene = 0;

	void RenderScene(const Time& frame_time)
	{
		//Renderer::ClearWindow();

		// Render everything to the framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, s_Framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		unsigned int num_meshes = 0;
		unsigned int num_verts = 0;

		for (auto viewport_it : s_Viewports)
		{
			Viewport& viewport = viewport_it.second;
			GLint x, y;
			viewport.GetPosition(x, y);
			GLsizei width, height;
			viewport.GetSize(width, height);
			glViewport(x, y, width, height);

			if (s_ActiveCamera)
			{
				ShaderManager::SetUniformMatrix4fv("camera_projection", s_ActiveCamera->GetProjMatrix());
				ShaderManager::SetUniformMatrix4fv("camera_view", s_ActiveCamera->GetViewMatrix());

				glm::vec3 ambient_colour(1.0f, 0.0f, 1.0f);
				ShaderManager::SetUniform3fv("lighting_ambient_colour", ambient_colour);

				glm::vec3 diffuse_position(1.0f, 0.0f, 0.0f);
				ShaderManager::SetUniform3fv("lighting_directional_vector", diffuse_position);

				glm::vec3 diffuse_colour(0.5f, 0.5f, 0.5f);
				ShaderManager::SetUniform3fv("lighting_directional_colour", diffuse_colour);

				glm::vec3 camera_pos = s_ActiveCamera->GetPosition();
				ShaderManager::SetUniform3fv("camera_position", camera_pos);

				for (auto it : m_RenderablesInScene)
				{
					if (it->IsActive())
					{
						it->Render();
						num_meshes += it->GetNumMeshes();
						num_verts += it->GetNumVerts();
					}
				}

				// Skybox.
				glm::mat4 skybox_view = glm::mat4(glm::mat3(s_ActiveCamera->GetViewMatrix()));
				glDepthFunc(GL_LEQUAL);
				ShaderManager::SetActiveShader(s_SkyboxShader);
				ShaderManager::SetUniformMatrix4fv("camera_view", skybox_view, s_SkyboxShader);
				glBindVertexArray(s_skyboxVAO);
				glBindTexture(GL_TEXTURE_CUBE_MAP, s_CubemapTexture);
				glActiveTexture(GL_TEXTURE0);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glDepthFunc(GL_LESS);
				//ShaderManager::SetUniformMatrix4fv("camera_view", s_ActiveCamera->GetViewMatrix());

				DebugDraw::Render();

				// 2D stuff, draw this last.
				ScreenSpaceRenderer::Render();
				DebugPanels::Render();
			}

			if (num_verts > s_PeakVertsInScene)
			{
				s_PeakVertsInScene = num_verts;
				std::stringstream str;
				str << "New peak verts: " << s_PeakVertsInScene << " (" << num_meshes << " meshes in scene)";
				Logging::Log("Renderer", str.str());
			}
		}

		//// second pass

		ShaderManager::SetActiveShader(s_PostprocessShader);
		GLuint uniTex = glGetUniformLocation(s_PostprocessShader, "tex");
		glUniform1i(uniTex, 0);
		GLuint uniTint = glGetUniformLocation(s_PostprocessShader, "tint");
		static float alpha = 1.0f;
		const float OFFSET = 0.02f;
		static float offset = -OFFSET;
		static glm::vec4 tint(1.0f, 1.0f, 1.0f, 1.0f);
		tint[3] = alpha;
		alpha += offset;
		if (alpha <= 0.0f)
		{
			offset = OFFSET;
		}
		if (alpha >= 1.0f)
		{
			tint[0] = rand() / (float)RAND_MAX;
			tint[1] = rand() / (float)RAND_MAX;
			tint[2] = rand() / (float)RAND_MAX;
			tint[3] = alpha = 1.0f;
			offset = -OFFSET;
		}
		glUniform4fv(uniTint, 1, glm::value_ptr(tint));

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(s_QuadVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_TextureColourBuffer);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void RegisterRenderable(Renderable* renderable)
	{
		m_RenderablesInScene.insert(renderable);
	}

	void UnregisterRenderable(Renderable* renderable)
	{
		m_RenderablesInScene.erase(renderable);
	}

	void RegisterViewport(const Viewport& viewport, const std::string& viewport_name)
	{
		s_Viewports[viewport_name] = viewport;
	}

	void UnregisterViewport(const std::string& viewport_name)
	{
		s_Viewports.erase(viewport_name);
	}

	Viewport& GetViewport(const std::string& viewport_name)
	{
		return s_Viewports[viewport_name];
	}
}
