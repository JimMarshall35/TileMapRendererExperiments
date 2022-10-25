#include "JanetScriptProceduralPopulater.h"
#include "JanetVmService.h"
#include "ProceduralHelpers.h"
#include <vector>
#include <limits.h>

u32 s_dataBeingPopulatedWidthTiles = 0;
u32 s_dataBeingPopulatedHeightTiles = 0;
u16* s_dataBeingPopulated = nullptr;

JANET_CFUN(Janet_SetTileAt) {
	janet_fixarity(argc, 3); // x, y, newval
	auto x = janet_getinteger(argv, 0); // x
	auto y = janet_getinteger(argv, 1); // y
	auto newVal = janet_getinteger(argv, 2); // newval
	if (x < 0) {
		janet_panicf("x: expected non-negative integer, got %v",
			argv[1]);
	}
	if (y < 0) {
		janet_panicf("y: expected non-negative integer, got %v",
			argv[1]);
	}
	s_dataBeingPopulated[y * s_dataBeingPopulatedWidthTiles + x] = newVal;
	return janet_wrap_nil();

}


JANET_CFUN(Janet_FloodLayerWeightedPallette)
{
	janet_fixarity(argc, 2);
	auto values = janet_gettuple(argv, 0);
	auto weights = janet_gettuple(argv, 1);

	auto valuesLength = janet_tuple_length(values);
	auto weightsLength = janet_tuple_length(weights);
	if (valuesLength != weightsLength) {
		janet_panic("weights and lengths did not match");
	}
	u32* cValues = (u32*)janet_smalloc(valuesLength * sizeof(u32));
	f32* cWeights = (f32*)janet_smalloc(weightsLength * sizeof(f32));

	auto valuesHead = janet_tuple_head(values);
	auto weightsHead = janet_tuple_head(weights);


	for (u32 i = 0; i < valuesLength; i++) {
		cValues[i] = (u32)valuesHead->data[i].number;
	}

	for (u32 i = 0; i < weightsLength; i++) {
		cWeights[i] = weightsHead->data[i].number;
	}

	auto numTiles = s_dataBeingPopulatedHeightTiles * s_dataBeingPopulatedWidthTiles;

	for (u32 i = 0; i < numTiles; i++) {
		s_dataBeingPopulated[i] = cValues[SelectWeighted(cWeights, weightsLength)];
	}

	janet_sfree(cValues);
	janet_sfree(cWeights);
	return janet_wrap_nil();
}

JANET_CFUN(Janet_RandomIntBetween) {
	janet_fixarity(argc, 2);

	auto min = janet_getinteger(argv, 0);
	auto max = janet_getinteger(argv, 1);
	if (min > USHRT_MAX || max > USHRT_MAX) {
		janet_panic("min and max must be between 0 and USHRT_MAX");
	}
	if (min < 0 || max < 0) {
		janet_panic("min and max must be > 0");
	}
	if (max < min) {
		janet_panic("max must be greater than min");
	}

	return janet_wrap_integer(GetRandomIntBetween(min, max));
}

JANET_CFUN(Janet_RandomU32Between) {
	janet_fixarity(argc, 2);

	auto min = janet_getinteger(argv, 0);
	auto max = janet_getinteger(argv, 1);
	if (min > UINT_MAX || max > UINT_MAX) {
		janet_panic("min and max must be between 0 and USHRT_MAX");
	}
	if (min < 0 || max < 0) {
		janet_panic("min and max must be > 0");
	}
	if (max < min) {
		janet_panic("max must be greater than min");
	}

	return janet_wrap_integer(GetRandomU32Between(min, max));
}

JanetScriptProceduralPopulater::JanetScriptProceduralPopulater(JanetVmService* vm)
	:m_vm(vm)
{
	vm->RegisterCFunction(Janet_SetTileAt, "set-tile-at");
	vm->RegisterCFunction(Janet_FloodLayerWeightedPallette, "flood-layer-weighted");
	vm->RegisterCFunction(Janet_RandomIntBetween, "random-u16-between");
	vm->RegisterCFunction(Janet_RandomU32Between, "random-u32-between");
	vm->TryLoadJanetFile("scripts\\test.lsp");
	//vm->CallJanetFunction(NULL, 0, "test-func");
}

void JanetScriptProceduralPopulater::PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles)
{
	s_dataBeingPopulated = layer;
	s_dataBeingPopulatedHeightTiles = layerHeightTiles;
	s_dataBeingPopulatedWidthTiles = layerWidthTiles;

	Janet args[3];
	args[0].number = layerNumber;
	args[1].number = layerWidthTiles;
	args[2].number = layerHeightTiles;

	m_vm->CallJanetFunction(args, 3, "generate-map");
}