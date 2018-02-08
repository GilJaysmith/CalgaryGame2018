#pragma once

#include <map>
#include <string>
#include <vector>

namespace ShaderManager
{
	void Initialize();

	void Terminate();

	unsigned int LoadProgram(const std::string& filename);

	void SetActiveShader(unsigned int shader_program);

	void SetUniformMatrix4fv(const std::string& uniform_name, const glm::mat4& v);
	void SetUniform3fv(const std::string& uniform_name, const glm::vec3& v);
}
