#include "AtlasQUadTreeBase.h"

void FAtlasQuadTreeBase::Reset() {
	if (!Nodes.empty()) {
		Nodes.resize(1);
		Node& Root = Nodes[0];
		Root.bOccupied = false;
		Root.bSplit = false;
		Root.Children[0] = Root.Children[1] = Root.Children[2] = Root.Children[3] = -1;
	}

	RemainingSpace = AtlasSize * AtlasSize;
}

void FAtlasQuadTreeBase::Clear() {
	Nodes.clear();
	RemainingSpace = AtlasSize * AtlasSize;
}

bool FAtlasQuadTreeBase::Split(int32 Idx) {
	if (Idx < 0 || Idx >= (int32)Nodes.size()) return false;

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
		Nodes[Idx].Children[i] = (int32)(Nodes.size() - 1);
	}
	Nodes[Idx].bSplit = true;
	return true;
}