#pragma once
#include "Math/Vector.h"
#include "Core/ClassTypes.h"

struct Node {
	// Dimensions
	FVector2		TopLeft;
	float			Resolution;

	// Data
	bool			bOccupied   = false;				// True if and only if this node is allocated to a shadow map by whole
	bool			bSplit	    = false;				// True if and only if this node has been split into a subtree
	int32			Children[4] = { -1, -1, -1, -1 };	// indices into Nodes[], -1 = no child
};

struct FAtlasRegion { uint32 X, Y, Size; bool bValid; };

class FAtlasQuadTreeBase {
public:
	// Initializes the head node of the atlas, and define the minimum resolution for the shadow maps to be allocated.
	virtual void Init(float InAtlasSize, float inMinShadowMapResolution) = 0;

	virtual void AddToBatch(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H) = 0;

	// Sorts the pending batch by evaluated resolution, then allocates all entries into the atlas.
	virtual TArray<FAtlasRegion> CommitBatch() = 0;

	// Called every frame to reset the atlas.
	void Reset();

	// Hard clear including the root node
	void Clear();

	// Accessor
	float GetAtlasSize() const { return AtlasSize; }

protected:
	// Allocates the node at NodeIdx and returns the corresponding atlas region. Returns invalid region if the node is occupied or too small.
	virtual FAtlasRegion AllocateNode(int32 NodeIdx, uint32 RequestedSize) = 0;

	// Ranks the importance of the input light source based on its properties.
	virtual float EvaluateResolution(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H) const = 0;

	// Greedily splits the quadtree to find the best fit for the new shadow map
	bool  Split(int32 Idx);

protected:
	TArray<std::pair<FPointLightParams, float>> Batch;
	TArray<Node> Nodes;
	float		 AtlasSize = 4096.f;
	float		 MinShadowMapResolution = 64.f;

	float		 RemainingSpace = 4096.f * 4096.f;
};