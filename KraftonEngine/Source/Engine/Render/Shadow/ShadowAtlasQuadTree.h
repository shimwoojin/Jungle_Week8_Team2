#pragma once
#include "AtlasQuadTreeBase.h"

// Exclusive to spotlights
struct FSpotLightParams;


// Buddy allocation quadtree for shadow atlas management
class FShadowAtlasQuadTree : public FAtlasQuadTreeBase {
public:
	// Initializes the head node of the atlas, and define the minimum resolution for the shadow maps to be allocated.
	void Init(float InAtlasSize, float inMinShadowMapResolution) override;

	void AddToBatch(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H);

	// Sorts the pending batch by evaluated resolution, then allocates all entries into the atlas.
	TArray<FAtlasRegion> CommitBatch() override;

private:
	// Allocates the node at NodeIdx and returns the corresponding atlas region. Returns invalid region if the node is occupied or too small.
	FAtlasRegion AllocateNode(int32 NodeIdx, uint32 RequestedSize) override;

	// Ranks the importance of the input light source based on its properties.
	float EvaluateResolution(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H) const;

private:
	TArray<std::pair<FSpotLightParams, float>> Batch;
};