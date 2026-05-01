#include "ShapeComponent.h"

void UShapeComponent::ContributeSelectedVisuals(FScene& Scene) const
{
	DrawDebugShape(Scene, DebugShapeColor);
}
