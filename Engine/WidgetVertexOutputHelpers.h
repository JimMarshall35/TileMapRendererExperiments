#ifndef WIDGETVERTEXOUTPUTHELPERS_H
#define WIDGETVERTEXOUTPUTHELPERS_H
#include "Widget.h"
#include <cglm/cglm.h>
#include "Atlas.h"

typedef enum
{
	VL_TL,
	VL_TR,
	VL_BL,
	VL_BR,
	VT_NUM
}WidgetQuadVertexLocation;

struct WidgetQuad
{
	struct WidgetVertex v[4];
};

void PopulateWidgetQuadWholeSprite(struct WidgetQuad* pQuad, AtlasSprite* pSprt);
void PopulateWidgetQuad(struct WidgetQuad* pQuad, AtlasSprite* pSprt, vec2 subSpriteTL, vec2 subSpriteBR);
void* OutputWidgetQuad(VECTOR(struct WidgetVertex) pOutVerts, const struct WidgetQuad* pQuad);
void* OutputWidgetQuads(VECTOR(struct WidgetVertex) pOutVerts, const struct WidgetQuad* pQuad, int num);
void TranslateWidgetQuad(vec2 vector, struct WidgetQuad* pOutQuad);
void SizeWidgetQuad(vec2 size, struct WidgetQuad* pOutQuad);
float WidgetQuadWidth(const struct WidgetQuad* pInQuad);
float WidgetQuadHeight(const struct WidgetQuad* pInQuad);
void ScaleWidgetQuad(float scaleX, float scaleY, struct WidgetQuad* pOutQuad);
void SetWidgetQuadColour(struct WidgetQuad* pQuad, float r, float g, float b, float a);

#endif