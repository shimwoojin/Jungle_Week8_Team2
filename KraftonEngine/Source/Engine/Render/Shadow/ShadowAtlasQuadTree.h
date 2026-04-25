#pragma once
#include "Math/Vector.h"
#include "Core/ClassTypes.h"

#include <queue>

// Exclusive to spotlights
struct FLightInfo;

//enum class EShadowMapSizeUpperBound : uint8 {
//	Size64		= 64,
//	Size128		= 128,
//	Size256		= 256,
//	Size512		= 512,
//	Size1024	= 1024,
//	Size2048	= 2048
//};

struct Node {
	// Dimensions
	FVector2		TopLeft;
	float			Resolution;			// Minimum resolution = 64 x 64

	// Data
	bool			bOccupied = false;	// True if and only if this node is allocated to a shadow map by whole
	bool			bSplit    = false;
	int32			Children[4] = {-1, -1, -1, -1};		// indices into Nodes[], -1 = no child
};

// Buddy allocation quadtree for shadow atlas management
class FShadowAtlasQuadTree {
public:
	void Init(float InAtlasSize, float inMinShadowMapResolution);

	// Try allocating the uv region for the input light source
	void Add(FLightInfo& InLightInfo);

	void Clear();

private:
	// Greedily splits the quadtree to find the best fit for the new shadow map
	void  SplitQuadTree();

	// Ranks the importance of the input light source based on its properties.
	float EvaluateLightImportance(const FLightInfo& InLightInfo);


private:
	std::queue<Node> NodeQueue;	// Queue for breadth-first traversal of the quadtree
	TArray<Node> Nodes;
	
	float MinShadowMapResolution;
};