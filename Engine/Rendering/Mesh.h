#pragma once

#include <map>

#include "Engine/Math/AABB.h"

struct MeshNode;
struct SubMesh;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadFromYaml(const std::string& filename);
	void Render(const glm::mat4& world_transform, const glm::vec4& tint);

	unsigned int GetNumVerts() const;

protected:

	std::string m_MeshName;
	MeshNode* m_RootNode;
	std::vector<SubMesh*> m_SubMeshes;
	unsigned int m_NumVerts;
	math::AABB m_AABB;
};