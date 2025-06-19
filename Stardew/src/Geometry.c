#include "Geometry.h"

bool Ge_PointInAABB(float pointX, float pointY, GeomRect rect)
{
    return pointX >= rect[GR_TLX] && pointX <= rect[GR_BRX] &&
        pointY >= rect[GR_TLY] && pointY <= rect[GR_BRY];
}
