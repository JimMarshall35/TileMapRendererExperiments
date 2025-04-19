#include "WidgetVertexOutputHelpers.h"


void PopulateWidgetQuadWholeSprite(struct WidgetQuad* pQuad, AtlasSprite* pSprt)
{
	vec2 tl = { 0,0 };
	vec2 br = { pSprt->widthPx, pSprt->heightPx };
	PopulateWidgetQuad(pQuad, pSprt, tl, br);
}

void PopulateWidgetQuad(struct WidgetQuad* pQuad, AtlasSprite* pSprt, vec2 subSpriteTL, vec2 subSpriteBR)
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


void* OutputWidgetQuad(VECTOR(struct WidgetVertex) pOutVerts, const struct WidgetQuad* pQuad)
{
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
		pOutVerts = VectorPush(pOutVerts, &pQuad->v[index]);
	}
	return pOutVerts;
}


void TranslateWidgetQuad(vec2 vector, struct WidgetQuad* pOutQuad)
{
	for (int i = 0; i < 4; i++)
	{
		pOutQuad->v[i].x += vector[0];
		pOutQuad->v[i].y += vector[1];
	}
}

// keeps TL where it is and sets a new size, expanding to the right and downwards
void SizeWidgetQuad(vec2 size, struct WidgetQuad* pOutQuad)
{
	pOutQuad->v[VL_TR].x = pOutQuad->v[VL_TL].x + size[0];
	pOutQuad->v[VL_BR].x = pOutQuad->v[VL_BL].x + size[0];

	pOutQuad->v[VL_BL].y = pOutQuad->v[VL_TL].y + size[1];
	pOutQuad->v[VL_BR].y = pOutQuad->v[VL_TR].y + size[1];
}

float WidgetQuadWidth(const struct WidgetQuad* pInQuad)
{
	return pInQuad->v[VL_TR].x - pInQuad->v[VL_TL].x;
}

float WidgetQuadHeight(const struct WidgetQuad* pInQuad)
{
	return pInQuad->v[VL_BR].y - pInQuad->v[VL_TR].y;
}

void ScaleWidgetQuad(float scaleX, float scaleY, struct WidgetQuad* pOutQuad)
{
	float w = WidgetQuadWidth(pOutQuad);
	float h = WidgetQuadHeight(pOutQuad);
	float newW = w * scaleX;
	float newH = h * scaleY;
	vec2 sz = { newW, newH };
	SizeWidgetQuad(sz, pOutQuad);
}
