#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <list>
#include "BasicTypedefs.h"

/*
*	code based on this: 
*	https://www.youtube.com/watch?v=ASAowY6yJII&t=1721s
*	javidx9
*/

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

#define MAX_DEPTH 8

template<typename T>
class StaticQuadTree
{
public:
	StaticQuadTree(const Rect& size, const u32 depth = 0)
	{
		m_depth = depth;
		Resize(size);
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
					return;
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

private:
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

protected:
	u32 m_depth = 0;
	Rect m_rect;
	Rect m_childrenTLBRs[4];
	StaticQuadTree<T>* m_children[4];

	std::vector<std::pair<Rect, T>> m_items;
};