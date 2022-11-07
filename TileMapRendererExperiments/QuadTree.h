#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <list>
#include "BasicTypedefs.h"

/*
*	code based (mostly just copied direct) on this: 
*	https://www.youtube.com/watch?v=ASAowY6yJII&t=1721s
*	javidx9
*/

#define MAX_DEPTH 8


struct Rect {
	glm::vec2 pos;
	glm::vec2 dims;
	inline float Left() const {
		return pos.x;
	}
	inline float Top() const {
		return pos.y;
	}
	inline float Right() const {
		return pos.x + dims.x;
	}
	inline float Bottom() const {
		return pos.y + dims.y;
	}
	bool Contains(const Rect& otherRect) const {
		if (otherRect.Left() >= Left() && otherRect.Right() <= Right()) {
			if (otherRect.Top() >= Top() && otherRect.Bottom() <= Bottom()) {
				return true;
			}
		}
		return false;
	}
	bool Overlaps(const Rect& r2) const {
		return (pos.x < r2.pos.x + r2.dims.x && pos.x + dims.x >= r2.pos.x &&
			pos.y < r2.pos.y + r2.dims.y && pos.y + dims.y >= r2.pos.y);
	}
};

template<typename T>
struct QuadTreeItemLocation {
	typename std::list<std::pair<Rect, T>>* container;
	typename std::list<std::pair<Rect, T>>::iterator iterator;
};


template<typename T>
class StaticQuadTree
{
	// same as dynamic but uses a vector as its container
	using StaticQuadTreeContainerType = std::vector<std::pair<Rect, T>>;
public:
	StaticQuadTree(const Rect& size, const u32 depth = 0)
	{
		m_depth = depth;
		Resize(size);
	}

	~StaticQuadTree() {
		Clear();
	}

	void Resize(const Rect& tlbr) {
		Clear();
		m_rect = tlbr;
		auto childSize = m_rect.dims * 0.5f;


		m_childrenTLBRs[0] = Rect{ m_rect.pos, childSize };
		m_childrenTLBRs[1] = Rect{ glm::vec2{m_rect.pos.x + childSize.x, m_rect.pos.y},childSize };
		m_childrenTLBRs[2] = Rect{ glm::vec2{m_rect.pos.x, m_rect.pos.y + childSize.y},childSize };
		m_childrenTLBRs[3] = Rect{ m_rect.pos + childSize, childSize };

	}
	void Clear() {
		m_items.clear();
		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				m_children[i]->Clear();
				delete m_children[i];
			}
		}
	}
	size_t Size() const {
		size_t count = m_items.size();
		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				count += m_children[i]->Size();
			}
		}
		return count;
	}

	void Insert(const T& item, const Rect& itemsize) {
		for (int i = 0; i < 4; i++) {
			if (m_childrenTLBRs[i].Contains(itemsize)) {
				if (m_depth + 1 < MAX_DEPTH) {
					if (!m_children[i]) {
						m_children[i] = new StaticQuadTree<T>(m_childrenTLBRs[i], m_depth + 1);
					}
					m_children[i]->Insert(item, itemsize);
				}
			}
		}
		m_items.push_back({ itemsize, item });
	}

	std::list<T> Search(const Rect& area) {
		std::list<T> listItems;
		Search(area, listItems);
		return listItems;
	}

	void Search(const Rect& area, std::list<T>& output) {
		for (const auto& p : m_items) {
			if (area.Overlaps(p.first)) {
				output.push_back(p.second);
			}
		}

		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				if (area.Contains(m_childrenTLBRs[i])) {
					m_children[i]->Items(output);
				}
				else if (m_childrenTLBRs[i].Overlaps(area)) {
					m_children[i]->Search(area, output);
				}
			}
		}
	}

private:


	void Items(std::list<T>& listItems) const {
		for (const auto& p : m_items) {
			listItems.push_back(p.second);
		}
		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				m_children[i]->Items(listItems);
			}
		}
	}

private:
	u32 m_depth = 0;
	Rect m_rect;
	Rect m_childrenTLBRs[4];
	StaticQuadTree<T>* m_children[4];

	StaticQuadTreeContainerType m_items;
};

template<typename T>
class DynamicQuadTree
{
	using DynamicQuadTreeContainerType = std::list<std::pair<Rect, T>>;
public:
	DynamicQuadTree(const Rect& size, const u32 depth = 0)
	{
		m_depth = depth;
		Resize(size);
	}

	~DynamicQuadTree() {
		Clear();
	}

	void Resize(const Rect& tlbr) {
		Clear();
		m_rect = tlbr;
		auto childSize = m_rect.dims * 0.5f;

		 
		m_childrenTLBRs[0] = Rect{ m_rect.pos, childSize };
		m_childrenTLBRs[1] = Rect{ glm::vec2{m_rect.pos.x + childSize.x, m_rect.pos.y},childSize };
		m_childrenTLBRs[2] = Rect{ glm::vec2{m_rect.pos.x, m_rect.pos.y + childSize.y},childSize };
		m_childrenTLBRs[3] = Rect{ m_rect.pos + childSize, childSize };

	}
	void Clear() {
		m_items.clear();
		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				m_children[i]->Clear();
				delete m_children[i];
			}
		}
	}
	size_t Size() const {
		size_t count = m_items.size();
		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				count += m_children[i]->Size();
			}
		}
		return count;
	}

	QuadTreeItemLocation<T> Insert(const T& item, const Rect& itemsize) {
		for (int i = 0; i < 4; i++) {
			if (m_childrenTLBRs[i].Contains(itemsize)) {
				if (m_depth + 1 < MAX_DEPTH) {
					if (!m_children[i]) {
						m_children[i] = new DynamicQuadTree<T>(m_childrenTLBRs[i], m_depth + 1);
					}
					return m_children[i]->Insert(item, itemsize);
				}
			}
		}
		m_items.push_back({ itemsize, item });
		return QuadTreeItemLocation<T>{ &m_items, std::prev(m_items.end()) };
	}

	std::list<T> Search(const Rect& area) {
		std::list<T> listItems;
		Search(area, listItems);
		return listItems;
	}

	void Search(const Rect& area, std::list<T>& output) {
		for (const auto& p : m_items) {
			if (area.Overlaps(p.first)) {
				output.push_back(p.second);
			}
		}

		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				if (area.Contains(m_childrenTLBRs[i])) {
					m_children[i]->Items(output);
				}
				else if (m_childrenTLBRs[i].Overlaps(area)) {
					m_children[i]->Search(area, output);
				}
			}
		}
	}

private:
	void Items(std::list<T>& listItems) const {
		for (const auto& p : m_items) {
			listItems.push_back(p.second);
		}
		for (int i = 0; i < 4; i++) {
			if (m_children[i]) {
				m_children[i]->Items(listItems);
			}
		}
	}

private:
	u32 m_depth = 0;
	Rect m_rect;
	Rect m_childrenTLBRs[4];
	DynamicQuadTree<T>* m_children[4];

	DynamicQuadTreeContainerType m_items;
};

template <typename T>
struct QuadTreeItem {
	T item;
	QuadTreeItemLocation<typename std::list<QuadTreeItem<T>>::iterator> pItem;
};

/// <summary>
/// for objects that move - contains a remove function
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
class DynamicQuadTreeContainer {
private:
	
	using QuadTreeContainer = std::list<QuadTreeItem<T>>;
private:
	QuadTreeContainer m_allItems;
	DynamicQuadTree<typename QuadTreeContainer::iterator> m_root;
public:
	~DynamicQuadTreeContainer() {
		m_root.Clear();
	}
	DynamicQuadTreeContainer(const Rect& size, const u32 depth = 0)
	:m_root(size,depth) {

	}
	void resize(const Rect& area) {
		m_root.Resize(area);
	}
	size_t size() const {
		return m_allItems.size();
	}
	bool empty() const {
		return m_allItems.empty();
	}

	typename QuadTreeContainer::const_iterator cbegin() {
		return m_allItems.cbegin();
	}
	typename QuadTreeContainer::const_iterator cend() {
		return m_allItems.cend();
	}
	void insert(const T& item, const Rect& itemSize) {
		QuadTreeItem<T> newItem;
		newItem.item = item;

		m_allItems.push_back(newItem);
		m_allItems.back().pItem = m_root.Insert(std::prev(m_allItems.end()), itemSize);
	}

	void remove(typename QuadTreeContainer::iterator& item) {
		item->pItem.container->erase(item->pItem.iterator);
		m_allItems.erase(item);
	}

	std::list<typename QuadTreeContainer::iterator> search(const Rect& rect) {
		std::list<typename QuadTreeContainer::iterator> listItemPointers;
		m_root.Search(rect, listItemPointers);
		return listItemPointers;
	}
};

/// <summary>
/// uses vectors to store the actual objects - for static objects.
/// no remove - only insert and clear
/// </summary>
template<typename T>
class StaticQuadTreeContainer {
	using QuadTreeContainer = std::vector<QuadTreeItem<T>>;
private:
	QuadTreeContainer m_allItems;
	StaticQuadTree<typename QuadTreeContainer::iterator> m_root;
public:
	StaticQuadTreeContainer(const Rect& size, const u32 depth = 0)
		:m_root(size, depth) {

	}
	~StaticQuadTreeContainer() {
		m_root.Clear();
	}

	void resize(const Rect& area) {
		m_root.Resize(area);
	}
	size_t size() const {
		return m_allItems.size();
	}
	bool empty() const {
		return m_allItems.empty();
	}

	typename QuadTreeContainer::const_iterator cbegin() {
		return m_allItems.cbegin();
	}
	typename QuadTreeContainer::const_iterator cend() {
		return m_allItems.cend();
	}
	void insert(const T& item, const Rect& itemSize) {
		QuadTreeItem<T> newItem;
		newItem.item = item;

		m_allItems.push_back(newItem);
		m_allItems.back().pItem = m_root.Insert(std::prev(m_allItems.end()), itemSize);
	}
	std::list<typename QuadTreeContainer::iterator> search(const Rect& rect) {
		std::list<typename QuadTreeContainer::iterator> listItemPointers;
		m_root.Search(rect, listItemPointers);
		return listItemPointers;
	}
};