#include "LuaBindings.h"
#include "SolInclude.h"
#include "LuaHandles.h"
#include "LuaBindingHelper.h"
#include "LuaWorldLibrary.h"
#include "LuaPropertyBridge.h"

#include "Component/CameraComponent.h"
#include "Component/SceneComponent.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static constexpr float RAD2DEG = 180.0f / M_PI;
static constexpr float DEG2RAD = M_PI / 180.0f;

namespace
{
	FLuaActorComponentHandle MakeActorComponentHandle(UActorComponent* Component)
	{
		FLuaActorComponentHandle Handle;
		if (Component)
		{
			Handle.UUID = Component->GetUUID();
		}
		return Handle;
	}

	FLuaSceneComponentHandle MakeSceneComponentHandle(USceneComponent* Component)
	{
		FLuaSceneComponentHandle Handle;
		if (Component)
		{
			Handle.UUID = Component->GetUUID();
		}
		return Handle;
	}

	void SetComponentWorldRotation(USceneComponent* Component, const FRotator& WorldRotation)
	{
		if (!Component)
		{
			return;
		}

		USceneComponent* Parent = Component->GetParent();
		if (!Parent)
		{
			Component->SetRelativeRotation(WorldRotation);
			return;
		}

		FMatrix DesiredWorld = WorldRotation.ToMatrix();
		FMatrix Relative = DesiredWorld * Parent->GetWorldMatrix().GetInverse();
		Component->SetRelativeRotation(Relative.ToRotator());
	}
}

void RegisterCameraComponentBinding(sol::state& Lua)
{
	Lua.new_usertype<FLuaCameraComponentHandle>(
		"CameraComponent",

		sol::no_constructor,

		LUA_HANDLE_COMMON(FLuaCameraComponentHandle),

		"FOVDegrees",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetFOV() * RAD2DEG : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float Degrees)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.FOVDegrees Access.");
					return;
				}
				C->SetFOV(Degrees * DEG2RAD);
			}
		),

		"FOVRadians",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetFOV() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float Radians)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.FOVRadians Access.");
					return;
				}
				C->SetFOV(Radians);
			}
		),

		"AspectRatio",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetAspectRatio() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float AspectRatio)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.AspectRatio Access.");
					return;
				}
				C->SetAspectRatio(AspectRatio);
			}
		),

		"NearZ",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetNearPlane() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float NearZ)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.NearZ Access.");
					return;
				}
				C->SetNearPlane(NearZ);
			}
		),

		"NearClip",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetNearPlane() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float NearZ)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.NearClip Access.");
					return;
				}
				C->SetNearPlane(NearZ);
			}
		),

		"FarZ",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetFarPlane() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float FarZ)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.FarZ Access.");
					return;
				}
				C->SetFarPlane(FarZ);
			}
		),

		"FarClip",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetFarPlane() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float FarZ)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.FarClip Access.");
					return;
				}
				C->SetFarPlane(FarZ);
			}
		),

		"Orthographic",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> bool
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->IsOrthogonal() : false;
			},
			[](const FLuaCameraComponentHandle& Self, bool bOrtho)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.Orthographic Access.");
					return;
				}
				C->SetOrthographic(bOrtho);
			}
		),

		"OrthoWidth",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> float
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetOrthoWidth() : 0.0f;
			},
			[](const FLuaCameraComponentHandle& Self, float Width)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.OrthoWidth Access.");
					return;
				}
				C->SetOrthoWidth(Clamp(Width, 0.1f, 100000.0f));
			}
		),

		"WorldLocation",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> FVector
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetWorldLocation() : FVector::ZeroVector;
			},
			[](const FLuaCameraComponentHandle& Self, const FVector& V)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.WorldLocation Access.");
					return;
				}
				C->SetWorldLocation(V);
			}
		),

		"RelativeLocation",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> FVector
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetRelativeLocation() : FVector::ZeroVector;
			},
			[](const FLuaCameraComponentHandle& Self, const FVector& V)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.RelativeLocation Access.");
					return;
				}
				C->SetRelativeLocation(V);
			}
		),

		"RelativeRotation",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> FRotator
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetRelativeRotation() : FRotator();
			},
			[](const FLuaCameraComponentHandle& Self, const FRotator& R)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.RelativeRotation Access.");
					return;
				}
				C->SetRelativeRotation(R);
			}
		),

		"WorldRotation",
		sol::property(
			[](const FLuaCameraComponentHandle& Self) -> FRotator
			{
				UCameraComponent* C = Self.Resolve();
				return C ? C->GetWorldMatrix().ToRotator() : FRotator();
			},
			[](const FLuaCameraComponentHandle& Self, const FRotator& R)
			{
				UCameraComponent* C = Self.Resolve();
				if (!C)
				{
					UE_LOG("[Lua] Invalid CameraComponent.WorldRotation Access.");
					return;
				}
				SetComponentWorldRotation(C, R);
			}
		),

		"AsComponent",
		[](const FLuaCameraComponentHandle& Self)
		{
			return MakeActorComponentHandle(Self.Resolve());
		},

		"AsScene",
		[](const FLuaCameraComponentHandle& Self)
		{
			return MakeSceneComponentHandle(Self.Resolve());
		},

		"ListProperties",
		[](const FLuaCameraComponentHandle& Self, sol::this_state State)
		{
			return FLuaPropertyBridge::ListProperties(State, Self.Resolve());
		},

		"HasProperty",
		[](const FLuaCameraComponentHandle& Self, const FString& Name)
		{
			return FLuaPropertyBridge::HasProperty(Self.Resolve(), Name);
		},

		"GetProperty",
		[](const FLuaCameraComponentHandle& Self, const FString& Name, sol::this_state State)
		{
			return FLuaPropertyBridge::GetProperty(State, Self.Resolve(), Name);
		},

		"SetProperty",
		[](const FLuaCameraComponentHandle& Self, const FString& Name, sol::object Value)
		{
			return FLuaPropertyBridge::SetProperty(Self.Resolve(), Name, Value);
		}
	);
}
