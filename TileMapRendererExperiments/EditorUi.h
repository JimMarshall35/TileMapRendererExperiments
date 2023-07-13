#pragma once
#include "BasicTypedefs.h"
#include "GameFramework.h"
#include "ECS.h"
#include <string>

#define MAX_TILES_PER_LUT_CASE 20

class TiledWorld;
class AtlasLoader;
class Camera2D;
class IFilesystem;
class EditorToolBase;
class CameraManager;
class EditorCamera;

struct ImGuiIO;
struct GLFWwindow;



class EditorUi
	:public DrawableLayerBase,
	public RecieveInputLayerBase,
	public UpdateableLayerBase
{
public:
	~EditorUi();
	EditorUi(TiledWorld* tiledWorld,
		AtlasLoader* atlasLoader, 
		ECS* ecsWorld, 
		const IFilesystem* fileSystem, 
		EditorToolBase** tools, 
		u32 numTools, 
		GLFWwindow* window,
		CameraManager* camManager);
private:
	void DoUiWindow();
	void MouseButtonCallback();
	void RightMouseButtonCallback();
private:
	TiledWorld* m_tiledWorld;
	AtlasLoader* m_atlasLoader;
	ECS* m_ecsWorld;
	EditorCamera* m_cam;
	Directions m_camMoveDirections;

	EditorToolBase** m_tools;
	u32 m_numTools;
	u32 m_selectedTool = 0;
	int m_layerToSet;
	ImGuiIO* m_io;
	GLFWwindow* m_window;
	CameraManager* m_camManager;
	bool m_wantMouseInput;
	bool m_wantKeyboardInput;
	bool m_dragging = false;
	float m_lastDeltaT;
	double m_lastMouseX;
	double m_lastMouseY;
	std::string m_oldCameraName;
public:
	// Inherited via DrawableLayerBase
	virtual void Draw(const Camera2D& camera) const override;
	virtual bool MasksPreviousDrawableLayer() const override;
	virtual std::string GetDrawableLayerName() const override;
	virtual void OnDrawablePush() override;
	virtual void OnDrawablePop() override;

	// Inherited via RecieveInputLayerBase
	virtual void ReceiveInput(const GameInput& input) override;
	virtual bool MasksPreviousInputLayer() const override;
	virtual std::string GetInputLayerName() const override;
	virtual void OnInputPush() override;
	virtual void OnInputPop() override;

	// Inherited via UpdateableLayerBase
	virtual void Update(float deltaT) override;
	virtual bool MasksPreviousUpdateableLayer() const override;
	virtual std::string GetUpdateableLayerName() const override;
	virtual void OnUpdatePush() override;
	virtual void OnUpdatePop() override;


};