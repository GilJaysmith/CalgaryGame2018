#pragma once

class Time;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual void Update(const Time& frame_time) = 0;
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjMatrix() const;
	virtual glm::vec3 GetPosition() const = 0;
	
	void SetActive(bool active);

protected:

	virtual void OnSetActive(bool active) = 0;

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjMatrix;
	bool m_Active;
};
