#pragma once
struct AtlasRect {
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
	bool Contains(const AtlasRect& otherRect) const {
		if (otherRect.Left() >= Left() && otherRect.Right() <= Right()) {
			if (otherRect.Top() >= Top() && otherRect.Bottom() <= Bottom()) {
				return true;
			}
		}
		return false;
	}

	bool Contains(const glm::vec2& point) const{
		return (point.x <= Right() && point.x >= Left())
			&& (point.y <= Bottom() && point.y >= Top());
	}

	bool Overlaps(const AtlasRect& r2) const {
		return (pos.x < r2.pos.x + r2.dims.x && pos.x + dims.x >= r2.pos.x &&
			pos.y < r2.pos.y + r2.dims.y && pos.y + dims.y >= r2.pos.y);
	}

	bool operator==(const AtlasRect& other)
	{
		return other.pos == pos && other.dims == dims;
	}
	bool operator!=(const AtlasRect& other)
	{
		return !((*this) == other);
	}
};