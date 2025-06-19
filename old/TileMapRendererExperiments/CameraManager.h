#pragma once
#include <string>
#include "GameFramework.h"
#include "WindowSizeChangedMessage.h"
class Camera2D;
class CameraManager
	:public GameFrameworkMessageRecipientBase<WindowSizedChangedArgs>
{
public:
	CameraManager(Camera2D** cameras, size_t numCameras);
	void SetActiveCamera(std::string name);
	inline Camera2D* GetActiveCamera() {
		return m_cameras[m_activeCamera];
	}
	Camera2D* GetCameraByName(std::string name);
	std::string GetActiveCameraName();
private:
	Camera2D** m_cameras;
	size_t m_numCameras;
	size_t m_activeCamera;
public:
	// Inherited via GameFrameworkMessageRecipientBase
	virtual void RecieveMessage(const WindowSizedChangedArgs& message) override;
};

