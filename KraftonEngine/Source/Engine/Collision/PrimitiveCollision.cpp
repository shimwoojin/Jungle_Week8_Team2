#include "Collision/PrimitiveCollision.h"
#include "Component/PrimitiveComponent.h"
#include "Component/Collision/BoxComponent.h"
#include "Component/Collision/SphereComponent.h"
#include "Component/Collision/CapsuleComponent.h"
#include "Math/MathUtils.h"

#include <algorithm>
#include <cmath>

namespace
{
	float GetMaxAbs(float A, float B, float C)
	{
		return std::max({ std::abs(A), std::abs(B), std::abs(C) });
	}

	float GetMaxAbs(float A, float B)
	{
		return std::max(std::abs(A), std::abs(B));
	}

	float GetScaledSphereRadius(const USphereComponent* Sphere)
	{
		const FVector Scale = Sphere->GetWorldScale();
		return Sphere->GetSphereRadius() * GetMaxAbs(Scale.X, Scale.Y, Scale.Z);
	}

	float GetScaledCapsuleRadius(const UCapsuleComponent* Capsule)
	{
		const FVector Scale = Capsule->GetWorldScale();
		return Capsule->GetCapsuleRadius() * GetMaxAbs(Scale.X, Scale.Y);
	}

	bool IntersectSegmentAABB(const FVector& Start, const FVector& End, const FBoundingBox& AABB)
	{
		const FVector Dir = End - Start;
		float TMin = 0.0f;
		float TMax = 1.0f;

		const float* StartPtr = &Start.X;
		const float* DirPtr = &Dir.X;
		const float* MinPtr = &AABB.Min.X;
		const float* MaxPtr = &AABB.Max.X;

		for (int Axis = 0; Axis < 3; ++Axis)
		{
			const float D = DirPtr[Axis];

			if (std::abs(D) < 1e-8f)
			{
				if (StartPtr[Axis] < MinPtr[Axis] || StartPtr[Axis] > MaxPtr[Axis])
				{
					return false;
				}
				continue;
			}

			float InvD = 1.0f / D;
			float T1 = (MinPtr[Axis] - StartPtr[Axis]) * InvD;
			float T2 = (MaxPtr[Axis] - StartPtr[Axis]) * InvD;

			if (T1 > T2) std::swap(T1, T2);

			TMin = std::max(TMin, T1);
			TMax = std::min(TMax, T2);

			if (TMin > TMax)
			{
				return false;
			}
		}

		return true;
	}
}