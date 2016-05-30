
#define GLM_SWIZZLE
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

Camera::Camera(float fovY, float aspectRatio, float near, float far)
	: m_speed(10),
	m_up(0,1,0),
	m_transform(1),
	m_view(1)
{
	setPerspective(fovY, aspectRatio, near, far);
}

Camera::~Camera()
{

}

void Camera::setPerspective(float fovY, float aspectRatio, float near, float far)
{
	m_projection = glm::perspective(fovY, aspectRatio, near, far);
	m_projectionView = m_projection * m_view;
}

void Camera::setLookAtFrom(const glm::vec3& from, const glm::vec3& to)
{
	m_view = glm::lookAt(from, to, glm::vec3(0, 1, 0));
	m_transform = glm::inverse(m_view);
	m_projectionView = m_projection * m_view;
}

void Camera::update(float deltaTime)
{
	GLFWwindow* window = glfwGetCurrentContext();

	static double
		mouseConserve = 0.575,
		moveConserve = 0.75;

	float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? deltaTime * m_speed * 4 : deltaTime * m_speed;	

	static float f0 = 0, f1 = 0, f2 = 0;

	// translate
	if (glfwGetKey(window,'W') == GLFW_PRESS)
		f2 -= frameSpeed;
	if (glfwGetKey(window,'S') == GLFW_PRESS)
		f2 += frameSpeed;
	if (glfwGetKey(window,'D') == GLFW_PRESS)
		f0 += frameSpeed;
	if (glfwGetKey(window,'A') == GLFW_PRESS)
		f0 -= frameSpeed;
	if (glfwGetKey(window,'Q') == GLFW_PRESS)
		f1 += frameSpeed;
	if (glfwGetKey(window,'E') == GLFW_PRESS)
		f1 -= frameSpeed;



	m_transform[3] += m_transform[0] * (f0 *= static_cast<float>(moveConserve));
	m_transform[3] += m_transform[1] * (f1 *= static_cast<float>(moveConserve));
	m_transform[3] += m_transform[2] * (f2 *= static_cast<float>(moveConserve));

	// check for rotation
	static bool sbMouseButtonDown = false;
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		static double siPrevMouseX = 0;
		static double siPrevMouseY = 0;

		if (sbMouseButtonDown == false)
		{
			sbMouseButtonDown = true;
			glfwGetCursorPos(window,&siPrevMouseX,&siPrevMouseY);
		}

		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window,&mouseX,&mouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		siPrevMouseX = mouseX;
		siPrevMouseY = mouseY;

		glm::mat4 mMat;

		static double accelX, accelY;		
		
		// pitch
		if (iDeltaY != 0)
		{
			accelY += iDeltaY;
		}

		// yaw
		if (iDeltaX != 0)
		{
			accelX += iDeltaX;
		}

		mMat = glm::axisAngleMatrix(m_transform[0].xyz(), (float)-(accelY *= mouseConserve) / 150.0f);
		m_transform[0] = mMat * m_transform[0];
		m_transform[1] = mMat * m_transform[1];
		m_transform[2] = mMat * m_transform[2];

		mMat = glm::axisAngleMatrix(m_up, (float)-(accelX *= mouseConserve) / 150.0f);
		m_transform[0] = mMat * m_transform[0];
		m_transform[1] = mMat * m_transform[1];
		m_transform[2] = mMat * m_transform[2];
	}
	else
	{
		sbMouseButtonDown = false;
	}

	m_view = glm::inverse(m_transform);
	m_projectionView = m_projection * m_view;
}

glm::vec3 Camera::screenPositionToDirection(float x, float y) const {
	
	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= m_projection[0][0];
	screenPos.y /= m_projection[1][1];

	return glm::normalize(m_transform * glm::vec4(screenPos, 0)).xyz();
}

glm::vec3 Camera::pickAgainstPlane(float x, float y, const glm::vec4& plane) const {

	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= m_projection[0][0];
	screenPos.y /= m_projection[1][1];

	glm::vec3 dir = glm::normalize(m_transform * glm::vec4(screenPos, 0)).xyz();

	float d = (plane.w - glm::dot(m_transform[3].xyz(), plane.xyz()) / glm::dot(dir, plane.xyz()));

	return m_transform[3].xyz() + dir * d;
}

void Camera::getFrustumPlanes(glm::vec4* planes) {

	auto transform = getProjectionView();

	// right side
	planes[0] = glm::vec4(transform[0][3] - transform[0][0],
		transform[1][3] - transform[1][0],
		transform[2][3] - transform[2][0],
		transform[3][3] - transform[3][0]);
	// left side
	planes[1] = glm::vec4(transform[0][3] + transform[0][0],
		transform[1][3] + transform[1][0],
		transform[2][3] + transform[2][0],
		transform[3][3] + transform[3][0]);
	// top
	planes[2] = glm::vec4(transform[0][3] - transform[0][1],
		transform[1][3] - transform[1][1],
		transform[2][3] - transform[2][1],
		transform[3][3] - transform[3][1]);
	// bottom
	planes[3] = glm::vec4(transform[0][3] + transform[0][1],
		transform[1][3] + transform[1][1],
		transform[2][3] + transform[2][1],
		transform[3][3] + transform[3][1]);
	// far
	planes[4] = glm::vec4(transform[0][3] - transform[0][2],
		transform[1][3] - transform[1][2],
		transform[2][3] - transform[2][2],
		transform[3][3] - transform[3][2]);
	// near
	planes[5] = glm::vec4(transform[0][3] + transform[0][2],
		transform[1][3] + transform[1][2],
		transform[2][3] + transform[2][2],
		transform[3][3] + transform[3][2]);
	// plane normalisation, based on length of normal
	for (int i = 0; i < 6; i++) {
		float d = glm::length(glm::vec3(planes[i]));
		planes[i] /= d;
	}
}