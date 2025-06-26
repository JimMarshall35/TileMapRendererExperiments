#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <stdbool.h>
#include <cglm/cglm.h>

enum Geom_RecCoords
{
	GR_TLX,
	GR_TLY,
	GR_BRX,
	GR_BRY,
};
typedef vec4 GeomRect;

bool Ge_PointInAABB(float pointX, float pointY, GeomRect rect);

#endif