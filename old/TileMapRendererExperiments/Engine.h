#pragma once
#include "NewRenderer.h"
#include "JSONPopulator.h"
#include "AtlasLoader.h"
#include "ECS.h"
#include "MetaAtlas.h"
#include "TileMapConfigOptions.h"
#include "DIContainer.h"
#include "QuadTree.h"
#include "WindowsFilesystem.h"
#include "ForthEngineSystem.h"

#include <memory>
#include <string>
#include <functional>
struct ImGuiIO;
struct GLFWwindow;
class IFilesystem;
class ForthEngineSystem;

typedef std::function<void(MetaAtlas*, ForthEngineSystem*)> StartupFunction;

enum class StartupFunctionType {
	None,
	Normal,
	ParallelWithPrevious,

	NumStartupFunctionTypes
};

class Engine
{
public:
	Engine(const TileMapConfigOptions& config, const NewRendererInitialisationInfo& rendererInit, const ForthEngineSystemInitArgs& forthInitArgs);
	int Run();
	void LoadJsonTileMap(const std::string& jsonFolder, const std::string& jsonFile);
	void StartLoadingTilesets();
	void StopLoadingTilesets(AtlasLoaderAtlasType atlasTypeToMake);
	bool TryLoadTileset(const TileSetInfo& tilesetInfo);
	bool TryLoadTilesetFromJSONInfo(const std::string& folder, const std::string& file);
	bool LoadMetaSpritesFromFile(const std::string& folder, const std::string& file);
	void RegisterStartupFunction(StartupFunctionType type, StartupFunction function);
private:
	static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	int StartupBoilerPlate(ImGuiIO*& io, GLFWwindow*& window);
	Directions processDirectionKeys(GLFWwindow* window);
	void processInput(GLFWwindow* window);
	void RunUserStartupFunctions(MetaAtlas* metaAtlas, ForthEngineSystem* forthEngineSystem);
	injector BuildDependencyInjector();
private:
	//injector m_injector;
	// di factories
	static std::unique_ptr<NewRenderer> RendererFactory();
	static std::unique_ptr<ECS> ECSFactory();
	static std::unique_ptr<MetaAtlas> MetaAtlasFactory(ECS* ecs);
	static std::unique_ptr<AtlasLoader> AtlasLoaderFactory();
	static std::unique_ptr<DynamicQuadTreeContainer<flecs::entity>> EntityQuadTreeFactory();
	static std::unique_ptr<WindowsFilesystem> IFileSystemFactory();
	static std::unique_ptr<ForthEngineSystem> ForthEngineSystemFactory(ForthEngineSystemInitArgs* args, ECS* ecs);
	static std::unique_ptr<ForthEngineSystemInitArgs> ForthEngineSystemInitArgsFactory();
private:
	ImGuiIO* m_io;
	GLFWwindow* m_window;
	JSONTiledWorldPopulator m_jsonTiledWorldPopulator;
	AtlasLoader* m_atlasLoader;
	MetaAtlas* m_metaAtlas;
	ECS* m_ecs;
	std::vector<std::pair<StartupFunction, StartupFunctionType>> m_startupFunctions;

	static u32 s_screenW;
	static u32 s_screenH;
	static TileMapConfigOptions s_tileMapConfigOptions;
	static NewRendererInitialisationInfo s_rendererInitialisationInfo;
	static NewRenderer* s_renderer;
	static ForthEngineSystemInitArgs s_forthSystemInitArgs;
	//di_config m_diConfig;
	//injector m_injector;
};

