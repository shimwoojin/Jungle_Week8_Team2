#include "ShadowAtlasQuadTree.h"
#include "Render/Types/GlobalLightParams.h"

#include <algorithm>

// Public functions
void FShadowAtlasQuadTree::Init(float InAtlasSize, float InMinShadowMapResolution) {
	if (InMinShadowMapResolution <= 0.f || InAtlasSize <= 0.f) {
		return;
	}
	if (InMinShadowMapResolution > InAtlasSize) {
		return;
	}

	AtlasSize = InAtlasSize;
	RemainingSpace = AtlasSize * AtlasSize;
	MinShadowMapResolution = InMinShadowMapResolution;
	Node RootNode = {};
	RootNode.TopLeft = FVector2(0.f, 0.f);
	RootNode.Resolution = InAtlasSize;

	Nodes.push_back(RootNode);
}

// Unused for now
//FAtlasRegion FShadowAtlasQuadTree::Add(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H) {
//	if (Nodes.empty()) return { 0, 0, 0, false };
//	float RequestedSize = EvaluateResolution(InLightInfo, CameraPos, Forward, FOV, H);
//	return AllocateNode(0, RequestedSize);
//}

void FShadowAtlasQuadTree::AddToBatch(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H) {
	Batch.push_back({InLightInfo, EvaluateResolution(InLightInfo, CameraPos, Forward, FOV, H)});
}

// Private helpers
FAtlasRegion FShadowAtlasQuadTree::AllocateNode(int32 NodeIdx, uint32 RequestedSize) {
	if (NodeIdx < 0
		|| NodeIdx >= Nodes.size()
		|| Nodes[NodeIdx].bOccupied
		|| RequestedSize < (uint32) (MinShadowMapResolution)
		|| !RemainingSpace) {
		// Invalid Node index
		return { 0, 0, 0, false };
	}

	if (RequestedSize * RequestedSize >= RemainingSpace && RequestedSize > MinShadowMapResolution)
		return AllocateNode(NodeIdx, RequestedSize / 2);

	Node node = Nodes[NodeIdx];
	if (node.bSplit) {
		for (int32 SubIdx : node.Children) {
			if (Nodes[SubIdx].bOccupied || Nodes[SubIdx].Resolution < RequestedSize) continue;

			// Greedily allocate the first child node that can fit the requested size
			FAtlasRegion AllocatedRegion = AllocateNode(SubIdx, RequestedSize);
			if (AllocatedRegion.bValid) {
				return AllocatedRegion;
			}
		}

		return { 0, 0, 0, false };
	}
	else {
		if (node.Resolution == RequestedSize) {
			RemainingSpace -= RequestedSize * RequestedSize;
			Nodes[NodeIdx].bOccupied = true;
			return { static_cast<uint32> (node.TopLeft.X), static_cast<uint32> (node.TopLeft.Y), static_cast<uint32>(node.Resolution), true };
		}
		else {

			// Try again after splitting
			if (Split(NodeIdx)) {
				return AllocateNode(NodeIdx, RequestedSize);
			}
		}
	}

	return { 0, 0, 0, false };
}


float FShadowAtlasQuadTree::EvaluateResolution(const FSpotLightParams& InLightInfo, FVector CameraPos, FVector Forward, float FOV, float H) const {
	if (InLightInfo.bCastShadows == false) return 0.f;

	FVector4 Color		 = InLightInfo.LightColor;
	float   r_sphere;
	FVector c_sphere;

	// If outer angle is less than 60 degrees
	if (InLightInfo.OuterConeCos >= 0.5) {
		r_sphere = InLightInfo.AttenuationRadius / (2 * InLightInfo.OuterConeCos);
		c_sphere = InLightInfo.Position + InLightInfo.Direction * r_sphere;
	} else {
		r_sphere = InLightInfo.AttenuationRadius;
		c_sphere = InLightInfo.Position;
	}

	auto z_view = (c_sphere - CameraPos).Dot(Forward);
	float z_guard = 5.f;
	z_view = z_view > z_guard ? z_view : z_guard;
	auto r_ndc = (r_sphere / z_view) / tanf(FOV / 2.f);
	auto r_pixel = r_ndc * H / 2.f;
	auto A_screen = 3.1415925f * r_pixel * r_pixel;

	float desired_res = sqrtf(A_screen) * (Color.X * 0.2126f + Color.Y * 0.7152f + Color.Z * 0.0722f) * InLightInfo.Intensity * InLightInfo.ShadowResolutionScale;
	//desired_res = desired_res > AtlasSize ? AtlasSize : desired_res;
	desired_res = static_cast<float>(RoundToNearestPowerOfTwo(static_cast<uint32>(desired_res)));
	return desired_res;
}

TArray<FAtlasRegion> FShadowAtlasQuadTree::CommitBatch() {
	const int32 N = static_cast<int32>(Batch.size());
	
	// Results are written back at original indices.
	TArray<int32> Order(N);
	for (int32 i = 0; i < N; ++i) Order[i] = i;
	std::sort(Order.begin(), Order.end(), [&](int32 A, int32 B) {
		return Batch[A].second > Batch[B].second;
	});

	TArray<FAtlasRegion> Results(N, { 0, 0, 0, false });
	for (int32 OrigIdx : Order) {
		FAtlasRegion AtlasRegion = AllocateNode(0, static_cast<uint32>(Batch[OrigIdx].second));
		Results[OrigIdx] = AtlasRegion;
	}

	Batch.clear();
	return Results;
}