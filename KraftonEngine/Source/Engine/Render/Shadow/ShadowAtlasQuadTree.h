#pragma once
#include "Math/Vector.h"
#include "Core/ClassTypes.h"

// Exclusive to spotlights
struct FLightInfo;

struct Node {
	// Dimensions
	FVector2		TopLeft;
	float			Resolution;		// Minimum resolution = 64 x 64

	// Data
	bool			bOccupied = false;	// True if and only if this node is allocated to a shadow map by whole
	TArray<Node>	Children;
};

// Buddy allocation quadtree for shadow atlas management
class FShadowAtlasQuadTree {
public:
	void Add(FLightInfo& InLightInfo);
	void Clear();

private:
	// Greedily splits the quadtree to find the best fit for the new shadow map
	void  SplitQuadTree();

	// Ranks the importance of the input light source based on its properties.
	float EvaluateLightImportance(const FLightInfo& InLightInfo);


private:
	TArray<Node> Nodes;
	
};