#pragma once
#include "Math/Vector.h"
#include "Core/ClassTypes.h"

struct Node {
	
};

class FShadowAtlasQuadTree {
public:
	void Add();
	void Clear();

private:
	TArray<Node> Nodes;
	
};