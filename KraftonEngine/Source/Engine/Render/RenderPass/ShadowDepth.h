#pragma once
#include "RenderPassBase.h"
#include "Render/Shadow/ShadowAtlasQuadTree.h"
#include "Render/Types/ForwardLightData.h"

class FShadowDepthPass : public FRenderPassBase {
public:
	FShadowDepthPass();
	void BeginPass(const FPassContext& Ctx) override;
	void EndPass(const FPassContext& Ctx) override;

private:
	FShadowAtlasQuadTree Quadtree;
	TArray<FLightAtlasEntry> AtlasEntries;  // CPU-side, rebuilt each frame
};