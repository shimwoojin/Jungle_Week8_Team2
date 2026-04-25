#include "ShadowAtlasQuadTree.h"
#include "Render/Pipeline/ForwardLightData.h"

// Public functions
void FShadowAtlasQuadTree::Init(float InAtlasSize, float InMinShadowMapResolution) {
	if (InMinShadowMapResolution <= 0.f || InAtlasSize <= 0.f) {
		return;
	}
	if (InMinShadowMapResolution < InAtlasSize) {
		return;
	}

	MinShadowMapResolution = InMinShadowMapResolution;
	Node RootNode		= {};
	RootNode.TopLeft	= FVector2(0.f, 0.f);
	RootNode.Resolution = InAtlasSize;
}

void FShadowAtlasQuadTree::Add(FLightInfo& InLightInfo) {
	// First check if there is a node of fitting size.
	

}

void FShadowAtlasQuadTree::Clear() {

}

// Private helpers
void FShadowAtlasQuadTree::SplitQuadTree() {

}

float FShadowAtlasQuadTree::EvaluateLightImportance(const FLightInfo& InLightInfo) {
	FVector  Direction = InLightInfo.Direction;
	FVector4 Color	   = InLightInfo.Color;
	float    Intensity = InLightInfo.Intensity;
	float    Radius    = InLightInfo.AttenuationRadius; 
	float    Falloff   = InLightInfo.FalloffExponent;

	return 0.f;
}