#include "ShadowAtlasQuadTree.h"
#include "Render/Pipeline/ForwardLightData.h"

// Public functions
void FShadowAtlasQuadTree::Init(float InAtlasSize, float InMinShadowMapResolution) {
	if (InMinShadowMapResolution <= 0.f || InAtlasSize <= 0.f) {
		return;
	}
	if (InMinShadowMapResolution > InAtlasSize) {
		return;
	}

	MinShadowMapResolution = InMinShadowMapResolution;
	Node RootNode		= {};
	RootNode.TopLeft	= FVector2(0.f, 0.f);
	RootNode.Resolution = InAtlasSize;

	Nodes.push_back(RootNode);
}

FAtlasRegion FShadowAtlasQuadTree::Add(FLightInfo& InLightInfo) {
	// First check if there is a node of fitting size.

	
	// Shrink the resolution until the requested resolution is less than or equal to any available nodes.


	// Split the node if a node of sufficient size is found but is too large for the requested resolution.


	// Otherwise, disregard the light source. It won't produce a shadow map this frame.
	return { 0, 0, 0, false };
}

void FShadowAtlasQuadTree::Reset() {
	NodeQueue = {};
	if (!Nodes.empty())
		Nodes.resize(1);
}

void FShadowAtlasQuadTree::Clear() {
	NodeQueue = {};
	Nodes.clear();
}

// Private helpers
FAtlasRegion FShadowAtlasQuadTree::AllocateNode(uint32 NodeIdx, uint32 RequestedSize) {

}

void FShadowAtlasQuadTree::Split(float Idx) {

}

float FShadowAtlasQuadTree::EvaluateResolution(const FLightInfo& InLightInfo) const {
	FVector  Direction = InLightInfo.Direction;
	FVector4 Color	   = InLightInfo.Color;
	float    Intensity = InLightInfo.Intensity;
	float    Radius    = InLightInfo.AttenuationRadius; 
	float    Falloff   = InLightInfo.FalloffExponent;

	// Spotlight evaluation


	return 0.f;
}