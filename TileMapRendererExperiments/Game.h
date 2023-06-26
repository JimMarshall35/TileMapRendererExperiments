#pragma once
#include "GameFramework.h"
#include "BasicTypedefs.h"
#include "WindowSizeChangedMessage.h"
#include "flecs.h"

class CameraManager;
class NewRenderer;
template <typename T>
class DynamicQuadTreeContainer;
struct MetaSprite;
class MetaAtlas;
class AtlasLoader;
class TiledWorld;
class GameCamera;

class Game
	:public UpdateableLayerBase,
	public DrawableLayerBase,
	public RecieveInputLayerBase,
	public GameFrameworkMessageRecipientBase<WindowSizedChangedArgs>
{
public:
	Game(
		CameraManager* camManager,
		NewRenderer* renderer,
		DynamicQuadTreeContainer<flecs::entity>* metaspritesQuadTree,
		MetaAtlas* metaAtlas,
		AtlasLoader* atlasLoader,
		TiledWorld* tiledWorld,
		u32 windowX,
		u32 windowY
	);

public:
	// Inherited via RecieveInputLayerBase
	virtual void ReceiveInput(const GameInput& input) override;
	virtual bool MasksPreviousInputLayer() const override;
	virtual std::string GetInputLayerName() const override;
	virtual void OnInputPush() override;
	virtual void OnInputPop() override;

	// Inherited via DrawableLayerBase
	virtual void Draw(const Camera2D& camera) const override;
	virtual bool MasksPreviousDrawableLayer() const override;
	virtual std::string GetDrawableLayerName() const override;
	virtual void OnDrawablePush() override;
	virtual void OnDrawablePop() override;

	// Inherited via UpdateableLayerBase
	virtual void Update(float deltaT) override;
	virtual bool MasksPreviousUpdateableLayer() const override;
	virtual std::string GetUpdateableLayerName() const override;
	virtual void OnUpdatePush() override;
	virtual void OnUpdatePop() override;

	// Inherited via GameFrameworkMessageRecipientBase
	virtual void RecieveMessage(const WindowSizedChangedArgs& message) override;
private:
	CameraManager* m_camManager;
	NewRenderer* m_renderer;
	DynamicQuadTreeContainer<flecs::entity>* m_metaspritesQuadTree;
	MetaAtlas* m_metaAtlas;
	AtlasLoader* m_atlasLoader;
	TiledWorld* m_tiledWorld;
	u32 m_windowWidth;
	u32 m_windowHeight;
	GameCamera* m_gameCamera;
	u32 m_tilesArrayTexture;
	float m_deltaT;
	u32 m_tiledWorldSizeX;
	u32 m_tiledWorldSizeY;
	
};

