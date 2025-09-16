#include "WidgetVertexOutputHelpers.h"
#include <stdlib.h>
#include <string.h>
#include "AssertLib.h"
#include "DrawContext.h"

static bool bClipRegionSet = false;
GeomRect gClipRect = { 0,0,0,0 };

void SetWidgetQuadColour(WidgetQuad* pQuad, float r, float g, float b, float a)
{
	for (int i = 0; i < 4; i++)
	{
		pQuad->v[i].r = r;
		pQuad->v[i].g = g;
		pQuad->v[i].b = b;
		pQuad->v[i].a = a;
	}
}

void SetClipRect(GeomRect clipRect)
{
	memcpy(gClipRect, clipRect, sizeof(GeomRect));
	bClipRegionSet = true;
}

void UnsetClipRect()
{
	bClipRegionSet = false;
}

void PopulateWidgetQuadWholeSprite(WidgetQuad* pQuad, AtlasSprite* pSprt)
{
	vec2 tl = { 0,0 };
	vec2 br = { pSprt->widthPx, pSprt->heightPx };
	PopulateWidgetQuad(pQuad, pSprt, tl, br);
}

void PopulateWidgetQuad(WidgetQuad* pQuad, AtlasSprite* pSprt, vec2 subSpriteTL, vec2 subSpriteBR)
{
	float subSpriteHeight = subSpriteBR[1] - subSpriteTL[1];
	float subSpriteWidth = subSpriteBR[0] - subSpriteTL[0];

	float uPerPxl = (pSprt->bottomRightUV_U - pSprt->topLeftUV_U) / (float)pSprt->widthPx;
	float vPerPxl = (pSprt->topLeftUV_V - pSprt->bottomRightUV_V) / (float)pSprt->heightPx;
	vec2 subSpriteTopLefUV = {
		pSprt->topLeftUV_U + subSpriteTL[0] * uPerPxl,
		pSprt->topLeftUV_V - subSpriteTL[1] * vPerPxl
	};
	vec2 subSpriteBottomRightUV = {
		subSpriteTopLefUV[0] + uPerPxl * subSpriteWidth,
		subSpriteTopLefUV[1] - vPerPxl * subSpriteHeight
	};

	pQuad->v[VL_TL].x = 0.0f;
	pQuad->v[VL_TL].y = 0.0f;
	pQuad->v[VL_TL].u = subSpriteTopLefUV[0];
	pQuad->v[VL_TL].v = subSpriteTopLefUV[1];
	pQuad->v[VL_TL].r = 1.0f; pQuad->v[VL_TL].g = 1.0f; pQuad->v[VL_TL].b = 1.0f; pQuad->v[VL_TL].a = 1.0f;

	pQuad->v[VL_TR].x = subSpriteWidth;
	pQuad->v[VL_TR].y = 0.0f;
	pQuad->v[VL_TR].u = subSpriteBottomRightUV[0];
	pQuad->v[VL_TR].v = subSpriteTopLefUV[1];
	pQuad->v[VL_TR].r = 1.0f; pQuad->v[VL_TR].g = 1.0f; pQuad->v[VL_TR].b = 1.0f; pQuad->v[VL_TR].a = 1.0f;

	pQuad->v[VL_BR].x = subSpriteWidth;
	pQuad->v[VL_BR].y = subSpriteHeight;
	pQuad->v[VL_BR].u = subSpriteBottomRightUV[0];
	pQuad->v[VL_BR].v = subSpriteBottomRightUV[1];
	pQuad->v[VL_BR].r = 1.0f; pQuad->v[VL_BR].g = 1.0f; pQuad->v[VL_BR].b = 1.0f; pQuad->v[VL_BR].a = 1.0f;

	pQuad->v[VL_BL].x = 0.0f;
	pQuad->v[VL_BL].y = subSpriteHeight;
	pQuad->v[VL_BL].u = subSpriteTopLefUV[0];
	pQuad->v[VL_BL].v = subSpriteBottomRightUV[1];
	pQuad->v[VL_BL].r = 1.0f; pQuad->v[VL_BL].g = 1.0f; pQuad->v[VL_BL].b = 1.0f; pQuad->v[VL_BL].a = 1.0f;

}

static bool AllCornerOutsideOfRegion(WidgetQuad* pQuad)
{
	int count = 0;
	for (int i = 0; i < VT_NUM; i++)
	{
		if (!Ge_PointInAABB(pQuad->v[i].x, pQuad->v[i].y, gClipRect))
		{
			count++;
		}
	}
	return count == 4;
}

static float ClipUV(float p0, float p1, float uv0, float uv1, float midpos)
{
	//EASSERT(p1 > p0);
	//EASSERT(midpos >= p0 && midpos <= p1);
	float s = p1 - p0;
	float s2 = midpos - p0;
	float t = s2 / s;
	float us = uv1 - uv0;
	return uv0 + (us)*t;
}

static bool ClipQuad(WidgetQuad* pQuad)
{
	if (AllCornerOutsideOfRegion(pQuad))
	{
		/*
			IF A QUAD IS GREATER THAN THE SIZE OF THE CANVAS IN BOTH DIMENSIONS IT WILL BE CLIPPED!!
		*/
		return false;
	}
	for (int i = 0; i < VT_NUM; i++)
	{
		WidgetVertex* pVert = &pQuad->v[i];
		if (pVert->x < gClipRect[0])
		{
			EASSERT(i == VL_TL || i == VL_BL);
			pVert->u = ClipUV(pQuad->v[VL_TL].x, pQuad->v[VL_TR].x, pQuad->v[VL_TL].u, pQuad->v[VL_TR].u, gClipRect[0]);
			pVert->x = gClipRect[0];
		}
		if (pVert->x > gClipRect[2])
		{
			EASSERT(i == VL_TR || i == VL_BR);
			pVert->u = ClipUV(pQuad->v[VL_TL].x, pQuad->v[VL_TR].x, pQuad->v[VL_TL].u, pQuad->v[VL_TR].u, gClipRect[2]);
			pVert->x = gClipRect[2];
		}
		if (pVert->y < gClipRect[1])
		{
			EASSERT(i == VL_TL || i == VL_TR);
			pVert->v = ClipUV(pQuad->v[VL_TL].y, pQuad->v[VL_BL].y, pQuad->v[VL_TL].v, pQuad->v[VL_BL].v, gClipRect[1]);
			pVert->y = gClipRect[1];
		}
		if (pVert->y > gClipRect[3])
		{
			EASSERT(i == VL_BL || i == VL_BR);
			pVert->v = ClipUV(pQuad->v[VL_TL].y, pQuad->v[VL_BL].y, pQuad->v[VL_TL].v, pQuad->v[VL_BL].v, gClipRect[3]);
			pVert->y = gClipRect[3];
		}
	}
	return true;
}

void* OutputWidgetQuad(VECTOR(WidgetVertex) pOutVerts, const WidgetQuad* pQuad)
{
	WidgetQuad cpy;
	memcpy(&cpy, pQuad, sizeof(WidgetQuad));

	if (bClipRegionSet)
	{
		if (!ClipQuad(&cpy))
		{
			return pOutVerts;
		}
	}

	const int numIndices = 6;
	const WidgetQuadVertexLocation indices[6] = {
		VL_TL,
		VL_TR,
		VL_BL,
		VL_TR,
		VL_BR,
		VL_BL
	};
	for (int i = 0; i < numIndices; i++)
	{
		int index = indices[i];
		pOutVerts = VectorPush(pOutVerts, &cpy.v[index]);
	}
	return pOutVerts;
}

void* OutputWidgetQuads(VECTOR(WidgetVertex) pOutVerts, const WidgetQuad* pQuads, int num)
{
	for (int i = 0; i < num; i++)
	{
		pOutVerts = OutputWidgetQuad(pOutVerts, &pQuads[i]);
	}
	return pOutVerts;
}


void TranslateWidgetQuad(vec2 vector, WidgetQuad* pOutQuad)
{
	for (int i = 0; i < 4; i++)
	{
		pOutQuad->v[i].x += vector[0];
		pOutQuad->v[i].y += vector[1];
	}
}

// keeps TL where it is and sets a new size, expanding to the right and downwards
void SizeWidgetQuad(vec2 size, WidgetQuad* pOutQuad)
{
	pOutQuad->v[VL_TR].x = pOutQuad->v[VL_TL].x + size[0];
	pOutQuad->v[VL_BR].x = pOutQuad->v[VL_BL].x + size[0];

	pOutQuad->v[VL_BL].y = pOutQuad->v[VL_TL].y + size[1];
	pOutQuad->v[VL_BR].y = pOutQuad->v[VL_TR].y + size[1];
}

float WidgetQuadWidth(const WidgetQuad* pInQuad)
{
	return pInQuad->v[VL_TR].x - pInQuad->v[VL_TL].x;
}

float WidgetQuadHeight(const WidgetQuad* pInQuad)
{
	return pInQuad->v[VL_BR].y - pInQuad->v[VL_TR].y;
}

void ScaleWidgetQuad(float scaleX, float scaleY, WidgetQuad* pOutQuad)
{
	float w = WidgetQuadWidth(pOutQuad);
	float h = WidgetQuadHeight(pOutQuad);
	float newW = w * scaleX;
	float newH = h * scaleY;
	vec2 sz = { newW, newH };
	SizeWidgetQuad(sz, pOutQuad);
}
