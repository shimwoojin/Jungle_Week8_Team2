#include "ShadowAtlasQuadTree.h"
#include "Render/Types/ForwardLightData.h"

// Public functions
void FShadowAtlasQuadTree::Init(float InAtlasSize, float InMinShadowMapResolution) {
	if (InMinShadowMapResolution <= 0.f || InAtlasSize <= 0.f) {
		return;
	}
	if (InMinShadowMapResolution > InAtlasSize) {
		return;
	}

	AtlasSize = InAtlasSize;
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
FAtlasRegion FShadowAtlasQuadTree::AllocateNode(int32 NodeIdx, uint32 RequestedSize) {
	if (NodeIdx < 0) {
		// Invalid Node index
		return { 0, 0, 0, false };
	}


}

bool FShadowAtlasQuadTree::Split(int32 Idx) {
	if (Idx < 0 || Idx <= Nodes.size()) return;

	float HalfRes = Nodes[Idx].Resolution / 2.f;
	if (HalfRes < MinShadowMapResolution) {
		// Reached the minimum resolution. Should not split further.
		return false;
	}

	FVector2 TL = Nodes[Idx].TopLeft;
	for (int i = 0; i < 4; i++) {
		Node NewNode = {};
		NewNode.Resolution = HalfRes;

		switch (i) {
		case (0): // Top-Left
			NewNode.TopLeft = TL;
			break;
		case (1): // Top-Right
			NewNode.TopLeft = TL + FVector2(HalfRes, 0);
			break;
		case (2): // Bottom-Left
			NewNode.TopLeft = TL + FVector2(0, HalfRes);
			break;
		case (3): // Bottom-Right
			NewNode.TopLeft = TL + FVector2(HalfRes, HalfRes);
			break;
		}
	}

	return true;
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