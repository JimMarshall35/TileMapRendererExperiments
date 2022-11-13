#pragma once
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