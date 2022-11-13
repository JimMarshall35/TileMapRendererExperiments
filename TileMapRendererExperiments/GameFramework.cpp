#include <functional>
#include "GameFramework.h"
#include "Camera2D.h"
#include "GameInput.h"
#include <iostream>
static size_t m_inputStackSize = 0;
static size_t m_drawableStackSize = 0;
static size_t m_updateableStackSize = 0;

RecieveInputLayerBase* GameFramework::m_inputStack[FRAMEWORK_STACKS_SIZE];
DrawableLayerBase* GameFramework::m_drawableStack[FRAMEWORK_STACKS_SIZE];
UpdateableLayerBase* GameFramework::m_updateableStack[FRAMEWORK_STACKS_SIZE];

size_t GameFramework::m_inputStackSize;
size_t GameFramework::m_drawableStackSize;
size_t GameFramework::m_updateableStackSize;
std::atomic<bool> GameFramework::m_newDataToReport;

GameFramework::GameFramework()
{
	m_newDataToReport = false;
}

void GameFramework::Update(double deltaT)
{
	if (m_updateableStackSize == 0) {
		return;
	}
	for (int i = 0; i < m_updateableStackSize; i++) {
		if (i == m_updateableStackSize - 1 || !m_updateableStack[i + 1]->MasksPreviousUpdateableLayer()) {
			m_updateableStack[i]->Update(deltaT);
		}
	}
}

void GameFramework::Draw(const Camera2D& camera)
{
	if (m_drawableStackSize == 0) {
		return;
	}
	for (int i = 0; i < m_drawableStackSize; i++) {
		if (i == m_drawableStackSize - 1 || !m_drawableStack[i + 1]->MasksPreviousDrawableLayer()) {
			m_drawableStack[i]->Draw(camera);
		}
	}
}

void GameFramework::RecieveInput(const GameInput& input)
{
	if (m_inputStackSize == 0) {
		return;
	}
	for (int i = 0; i < m_inputStackSize; i++) {
		if (i == m_inputStackSize - 1 || !m_inputStack[i + 1]->MasksPreviousInputLayer()) {
			m_inputStack[i]->ReceiveInput(input);
		}
	}
}




bool GameFramework::PushLayers(std::string name, GameLayerType whichLayers)
{
	// TODO: refactor to reduce duplication
	if (whichLayers & GameLayerType::Input) {
		bool hasFoundName = false;
		const auto& list = AutoList<RecieveInputLayerBase>::GetList();
		for (const auto layer : list) {
			if (layer->GetInputLayerName() == name) {
				if (!hasFoundName) {
					hasFoundName = true;
					PushInputLayer(layer);
				}
				else {
					std::cerr << "[GameFramework] Requested input layer " << name << " was found more than once in the auto list - this isn't allowed" << std::endl;
					return false;
				}
			}
		}
		if (!hasFoundName) {
			std::cerr << "[GameFramework] Requested input layer " << name << " was not found" << std::endl;
			return false;
		}
	}
	if (whichLayers & GameLayerType::Draw) {
		bool hasFoundName = false;
		const auto& list = AutoList<DrawableLayerBase>::GetList();
		for (const auto layer : list) {
			if (layer->GetDrawableLayerName() == name) {
				if (!hasFoundName) {
					hasFoundName = true;
					PushDrawableLayer(layer);
				}
				else {
					std::cerr << "[GameFramework] Requested draw layer " << name << " was found more than once in the auto list - this isn't allowed" << std::endl;
					return false;
				}
			}
		}
		if (!hasFoundName) {
			std::cerr << "[GameFramework] Requested draw layer " << name << " was not found" << std::endl;
			return false;
		}
	}
	if (whichLayers & GameLayerType::Update) {
		bool hasFoundName = false;
		const auto& list = AutoList<UpdateableLayerBase>::GetList();
		for (const auto layer : list) {
			if (layer->GetUpdateableLayerName() == name) {
				if (!hasFoundName) {
					hasFoundName = true;
					PushUpdatableLayer(layer);
				}
				else {
					std::cerr << "[GameFramework] Requested update layer " << name << " was found more than once in the auto list - this isn't allowed" << std::endl;
					return false;
				}
			}
		}
		if (!hasFoundName) {
			std::cerr << "[GameFramework] Requested update layer " << name << " was not found" << std::endl;
			return false;
		}
	}
	m_newDataToReport = true;
	return true;
}

bool GameFramework::PopLayers(GameLayerType whichLayers)
{
	// TODO: refactor to reduce duplication
	if (whichLayers & GameLayerType::Input) {
		PopInputLayer();
	}
	if (whichLayers & GameLayerType::Draw) {
		PopDrawableLayer();
	}
	if (whichLayers & GameLayerType::Update) {
		PopUpdatableLayer();
	}
	m_newDataToReport = true;

	return true;
}

void GameFramework::PushInputLayer(RecieveInputLayerBase* input)
{
	if (m_inputStackSize >= FRAMEWORK_STACKS_SIZE) {
		std::cerr << "FRAMEWORK_STACKS_SIZE exceeded - can't push\n";
		return;
	}
	m_inputStack[m_inputStackSize++] = input;
	input->OnInputPush();
}

RecieveInputLayerBase* GameFramework::PopInputLayer()
{
	if (m_inputStackSize == 0) {
		std::cerr << "m_inputStackSize is zero - can't pop\n";
		return nullptr;
	}
	auto top = m_inputStack[--m_inputStackSize];
	top->OnInputPop();
	return top;
}

void GameFramework::PushDrawableLayer(DrawableLayerBase* drawable)
{
	if (m_inputStackSize >= FRAMEWORK_STACKS_SIZE) {
		std::cerr << "FRAMEWORK_STACKS_SIZE exceeded - can't push\n";
		return;
	}
	m_drawableStack[m_drawableStackSize++] = drawable;
	drawable->OnDrawablePush();
}

const DrawableLayerBase* GameFramework::PopDrawableLayer()
{
	if (m_drawableStackSize == 0) {
		std::cerr << "m_drawableStackSize is zero - can't pop\n";
		return nullptr;
	}
	auto top = m_drawableStack[--m_drawableStackSize];
	top->OnDrawablePop();
	return top;
}

void GameFramework::PushUpdatableLayer(UpdateableLayerBase* updatable)
{
	if (m_inputStackSize >= FRAMEWORK_STACKS_SIZE) {
		std::cerr << "FRAMEWORK_STACKS_SIZE exceeded - can't push\n";
		return;
	}
	m_updateableStack[m_updateableStackSize++] = updatable;
	updatable->OnUpdatePush();
}

UpdateableLayerBase* GameFramework::PopUpdatableLayer()
{
	if (m_updateableStack == 0) {
		std::cerr << "m_updateableStack is zero - can't pop\n";
		return nullptr;
	}
	auto top = m_updateableStack[--m_updateableStackSize];
	top->OnUpdatePop();
	return top;
}

RecieveInputLayerBase* GameFramework::PeekInputLayer()
{
	return m_inputStack[m_inputStackSize - 1];
}

DrawableLayerBase* GameFramework::PeekDrawableLayer()
{
	return m_drawableStack[m_drawableStackSize - 1];
}

UpdateableLayerBase* GameFramework::PeekUpdatableLayer()
{
	return m_updateableStack[m_updateableStackSize - 1];
}

RecieveInputLayerBase** GameFramework::GetInputLayers()
{
	return m_inputStack;
}
DrawableLayerBase** GameFramework::GetDrawableLayers()
{
	return m_drawableStack;
}
UpdateableLayerBase** GameFramework::GetUpdatableLayers()
{
	return m_updateableStack;
}
size_t GameFramework::GetInputLayersSize()
{
	return m_inputStackSize;
}
size_t GameFramework::GetUpdatableLayersSize()
{
	return m_updateableStackSize;
}
size_t GameFramework::GetDrawableLayersSize()
{
	return m_drawableStackSize;
}
const bool GameFramework::NewDataToReport()
{
	return m_newDataToReport;
}
void GameFramework::AcknowledgeNewData()
{
	m_newDataToReport = false;
}