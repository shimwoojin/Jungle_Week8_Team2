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
	// First check if there is a node of a sufficient size.
	if (!Nodes.empty()) {
		NodeQueue.push(0);  // Traverse from the root node
	} else {
	    // Function called before initialization. Should not allocate
		return { 0, 0, 0, false };
	}

	// Find the tightest resolution available that can fit the requested resolution for the new shadow map.
	float BestResolution = 0.f;
	float RequestedResolution = EvaluateResolution(InLightInfo);
	while (!NodeQueue.empty()) {
		Node curr = Nodes[NodeQueue.front()];
		if (curr.bOccupied) {
			NodeQueue.pop();
			continue;
		}

		if (curr.Resolution >= RequestedResolution && curr.Resolution < BestResolution) {
			BestResolution = RequestedResolution;
		}

		NodeQueue.pop();

		for (uint8 i = 0; i < 4; i++) {
			if (curr.Children[i] >= 0) {
				NodeQueue.push(curr.Children[0]);
			}
		}
	}

	
	// Shrink the resolution until the requested resolution is less than or equal to any available nodes.
	// Otherwise, disregard the light source. It won't produce a shadow map this frame.
	NodeQueue = {};
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
	if (NodeIdx < 0 
		|| NodeIdx >= Nodes.size() 
		|| Nodes[NodeIdx].bOccupied) {
		// Invalid Node index
		return { 0, 0, 0, false };
	}

	Node node = Nodes[NodeIdx];
	if (node.bSplit) {
	    for (float SubIdx : node.Children) {
			// Greedily allocate the first child node that can fit the requested size
			FAtlasRegion AllocatedRegion = AllocateNode(SubIdx, RequestedSize);
			if (AllocatedRegion.bValid) {
				return AllocatedRegion;
			}
		}

		return { 0, 0, 0, false };
	} else {
	    if (node.Resolution == RequestedSize) {
			return { static_cast<uint32> (node.TopLeft.X), static_cast<uint32> (node.TopLeft.Y), static_cast<uint32>(node.Resolution), true};
		} else {

			// Try again after splitting
		    if (Split(NodeIdx)) {
			    return AllocateNode(NodeIdx, RequestedSize);
			}
		}
	}

	return { 0, 0, 0, false };
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
		Nodes.push_back(NewNode);
		Nodes[Idx].Children[i] = Nodes.size() - 1;
	}
	Nodes[Idx].bSplit = true;
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