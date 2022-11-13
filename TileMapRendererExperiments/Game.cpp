#include "Game.h"
#include "TileChunk.h"
#include "NewRenderer.h"
#include "QuadTree.h"
#include "MetaSpriteComponent.h"
#include "MetaAtlas.h"
#include "AtlasLoader.h"
#include "CameraManager.h"
#include "GameCamera.h"
#include "TiledWorld.h"

#define GAME_CAM_NAME "Game"


Game::Game(
    CameraManager* camManager,
    NewRenderer* renderer,
    DynamicQuadTreeContainer<MetaSprite>* metaspritesQuadTree,
    MetaAtlas* metaAtlas,
    AtlasLoader* atlasLoader,
    TiledWorld* tiledWorld,
    u32 windowX,
    u32 windowY
)
    :m_camManager(camManager),
    m_renderer(renderer),
    m_metaspritesQuadTree(metaspritesQuadTree),
    m_metaAtlas(metaAtlas),
    m_atlasLoader(atlasLoader),
    m_tiledWorld(tiledWorld),
    m_gameCamera((GameCamera*)m_camManager->GetCameraByName(GAME_CAM_NAME)),
    m_tilesArrayTexture(m_atlasLoader->GetArrayTextureByName("City tiles")),
    m_windowHeight(windowY),
    m_windowWidth(windowX)
{
    
}

void Game::ReceiveInput(const GameInput& input)
{
}

bool Game::MasksPreviousInputLayer() const
{
    return false;
}

std::string Game::GetInputLayerName() const
{
    return "Game";
}

void Game::OnInputPush()
{
}

void Game::OnInputPop()
{
}

void Game::Draw(const Camera2D& camera) const
{
    TileChunk::DrawVisibleChunks(m_tilesArrayTexture, *m_renderer, *m_camManager->GetActiveCamera(), *m_tiledWorld, m_windowWidth, m_windowHeight);

    auto camTLBR = m_gameCamera->GetTLBR();
    Rect r;
    r.pos.x = camTLBR.y;
    r.pos.y = camTLBR.x;

    r.dims.x = camTLBR.w - camTLBR.y;
    r.dims.y = camTLBR.z - camTLBR.x;
    auto vis = m_metaspritesQuadTree->search(r);
    for (const auto& sprite : vis) {
        m_renderer->DrawMetaSprite(
            sprite->item.handle,
            sprite->item.pos,
            { 1,1 },
            0,
            *m_metaAtlas,
            m_tilesArrayTexture,
            *m_camManager->GetActiveCamera());
    }
}

bool Game::MasksPreviousDrawableLayer() const
{
    return false;
}

std::string Game::GetDrawableLayerName() const
{
    return "Game";
}

void Game::OnDrawablePush()
{
    m_camManager->SetActiveCamera(GAME_CAM_NAME);
}

void Game::OnDrawablePop()
{
}

void Game::Update(float deltaT)
{
}

bool Game::MasksPreviousUpdateableLayer() const
{
    return false;
}

std::string Game::GetUpdateableLayerName() const
{
    return "Game";
}

void Game::OnUpdatePush()
{
}

void Game::OnUpdatePop()
{
}

void Game::RecieveMessage(const WindowSizedChangedArgs& message)
{
    m_windowHeight = message.newSizeY;
    m_windowWidth = message.newSizeX;
}
