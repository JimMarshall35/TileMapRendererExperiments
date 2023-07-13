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
#include "GameInput.h"
#include "Position.h"

#define GAME_CAM_NAME "Game"


Game::Game(
    CameraManager* camManager,
    NewRenderer* renderer,
    DynamicQuadTreeContainer<flecs::entity>* metaspritesQuadTree,
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
    m_windowWidth(windowX),
    m_tiledWorldSizeX(m_tiledWorld->GetMapWidth()),
    m_tiledWorldSizeY(m_tiledWorld->GetMapHeight())
{
}

void Game::ReceiveInput(const GameInput& input)
{
    switch (input.type) {
    case GameInputType::DirectionInput:
    {
        glm::vec2 moveVec = { 0,0 };
        bool move = false;
        if (input.data.direction.directions & Directions::UP) {
            moveVec.y = -1.0f;
            move = true;
        }
        if (input.data.direction.directions & Directions::DOWN) {
            moveVec.y = 1.0f;
            move = true;

        }
        if (input.data.direction.directions & Directions::LEFT) {
            moveVec.x = -1.0f;
            move = true;

        }
        if (input.data.direction.directions & Directions::RIGHT) {
            moveVec.x = 1.0f;
            move = true;

        }

        if (move) {
            m_gameCamera->FocusPosition += glm::normalize(moveVec) * 60.0f * m_deltaT;
            m_gameCamera->ClampPosition();
        }
        
        
    }
        break;
    case GameInputType::MouseScrollWheel:
        m_gameCamera->Zoom *= input.data.mouseScrollWheel.offset > 0 ? (1.1 * input.data.mouseScrollWheel.offset) : 0.9 / abs(input.data.mouseScrollWheel.offset);
        m_gameCamera->ClampPosition();
        break;
    }
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

    auto camTLBR = camera.GetTLBR();
    Rect r;
    r.pos.x = camTLBR.y;
    r.pos.y = camTLBR.x;

    r.dims.x = camTLBR.w - camTLBR.y;
    r.dims.y = camTLBR.z - camTLBR.x;
    auto vis = m_metaspritesQuadTree->search(r);
    for (const auto& qItem : vis) {
        
        auto entity = qItem->item;
        
        const MetaSpriteComponent* metaSprite = entity.get<MetaSpriteComponent>();

        if (metaSprite && metaSprite->ready) {
            m_renderer->DrawMetaSprite(
                metaSprite->handle,
                metaSprite->pos,
                { 1,1 },
                0,
                *m_metaAtlas,
                m_tilesArrayTexture,
                *m_camManager->GetActiveCamera());
        }
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
    m_deltaT = deltaT;
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
