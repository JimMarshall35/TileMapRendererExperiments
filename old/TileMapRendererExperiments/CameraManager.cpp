#include "CameraManager.h"
#include "Camera2D.h"

CameraManager::CameraManager(Camera2D** cameras, size_t numCameras)
:m_cameras(cameras),
m_numCameras(numCameras),
m_activeCamera(0) {

}

void CameraManager::SetActiveCamera(std::string name)
{
	for (int i = 0; i < m_numCameras; i++) {
		if (m_cameras[i]->Name() == name) {
			m_activeCamera = i;
			return;
		}
	}
}

Camera2D* CameraManager::GetCameraByName(std::string name)
{
	for (int i = 0; i < m_numCameras; i++) {
		if (m_cameras[i]->Name() == name) {
			return m_cameras[i];
		}
	}
}

std::string CameraManager::GetActiveCameraName()
{
	return m_cameras[m_activeCamera]->Name();
}

void CameraManager::RecieveMessage(const WindowSizedChangedArgs& message)
{
	for (int i = 0; i < m_numCameras; i++) {
		m_cameras[i]->SetWindowWidthAndHeight(message.newSizeX, message.newSizeY);
	}
}
