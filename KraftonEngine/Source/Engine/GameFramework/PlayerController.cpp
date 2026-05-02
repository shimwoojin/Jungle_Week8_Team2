#include "GameFramework/PlayerController.h"
#include "Component/CameraComponent.h"
#include "Component/ActorComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Serialization/Archive.h"
#include "GameFramework/World.h"
#include "GameFramework/Pawn.h"

#include <cstring>

IMPLEMENT_CLASS(APlayerController, AActor)


namespace
{
	constexpr int32 ControllerMovementFrameCount = 2;
	constexpr int32 ControllerLookModeCount = 3;

	int32 NormalizeMovementFrame(int32 Value)
	{
		return Value == static_cast<int32>(EControllerMovementFrame::World)
			? static_cast<int32>(EControllerMovementFrame::World)
			: static_cast<int32>(EControllerMovementFrame::Camera);
	}

	int32 NormalizeLookMode(int32 Value)
	{
		switch (Value)
		{
		case static_cast<int32>(EControllerLookMode::CameraOnly):
			return static_cast<int32>(EControllerLookMode::CameraOnly);
		case static_cast<int32>(EControllerLookMode::PawnYawPawnPitch):
			return static_cast<int32>(EControllerLookMode::PawnYawPawnPitch);
		default:
			return static_cast<int32>(EControllerLookMode::Auto);
		}
	}
}

static UCameraComponent* FindCameraOnActor(AActor* Target);

void APlayerController::Serialize(FArchive& Ar)
{
	AActor::Serialize(Ar);
	Ar << ControlRotation;
	Ar << MovementFrame;
	Ar << LookMode;

	if (Ar.IsLoading())
	{
		MovementFrame = NormalizeMovementFrame(MovementFrame);
		LookMode = NormalizeLookMode(LookMode);
	}
}

void APlayerController::GetEditableProperties(TArray<FPropertyDescriptor>& OutProps)
{
	AActor::GetEditableProperties(OutProps);

	static const char* MovementFrameNames[] = { "World", "Camera" };
	static const char* LookModeNames[] = { "Auto", "Camera Only", "Pawn Yaw + Pawn Pitch" };

	OutProps.push_back({
		"Movement Frame",
		EPropertyType::Enum,
		&MovementFrame,
		0.0f,
		0.0f,
		0.1f,
		MovementFrameNames,
		ControllerMovementFrameCount
	});

	OutProps.push_back({
		"Look Mode",
		EPropertyType::Enum,
		&LookMode,
		0.0f,
		0.0f,
		0.1f,
		LookModeNames,
		ControllerLookModeCount
	});
}

void APlayerController::PostEditProperty(const char* PropertyName)
{
	AActor::PostEditProperty(PropertyName);

	if (strcmp(PropertyName, "Movement Frame") == 0)
	{
		MovementFrame = NormalizeMovementFrame(MovementFrame);
	}
	else if (strcmp(PropertyName, "Look Mode") == 0)
	{
		LookMode = NormalizeLookMode(LookMode);
	}
}

void APlayerController::InitDefaultComponents()
{
	if (GetRootComponent())
	{
		return;
	}

	if (UStaticMeshComponent* Root = AddComponent<UStaticMeshComponent>())
	{
		SetRootComponent(Root);
	}
}

void APlayerController::SetMovementFrame(EControllerMovementFrame InFrame)
{
	MovementFrame = NormalizeMovementFrame(static_cast<int32>(InFrame));
}

void APlayerController::SetLookMode(EControllerLookMode InMode)
{
	LookMode = NormalizeLookMode(static_cast<int32>(InMode));
}

void APlayerController::EndPlay()
{
	UnPossess();
	ViewTarget = nullptr;
	AActor::EndPlay();
}

static FRotator MakeControlRotationFromCamera(const UCameraComponent* Camera)
{
	FRotator Rotation = Camera ? Camera->GetWorldMatrix().ToRotator() : FRotator();
	Rotation.Roll = 0.0f;
	return Rotation;
}

void APlayerController::Possess(APawn* InPawn)
{
	if (Pawn == InPawn)
	{
		// 같은 Pawn을 다시 Possess해도 ViewTarget은 Pawn으로 맞춘다.
		ViewTarget = InPawn;
		return;
	}

	UnPossess();

	Pawn = InPawn;
	ViewTarget = InPawn;

	if (Pawn)
	{
		Pawn->SetController(this);
		if (UCameraComponent* Camera = Pawn->FindPawnCamera())
		{
			ControlRotation = MakeControlRotationFromCamera(Camera);
		}
	}
}

void APlayerController::UnPossess()
{
	APawn* CurrentPawn = GetPawn();
	if (CurrentPawn && CurrentPawn->GetController() == this)
	{
		CurrentPawn->SetController(nullptr);
	}
	Pawn = nullptr;
}

APawn* APlayerController::GetPawn() const
{
	if (!Pawn || !IsAliveObject(Pawn))
	{
		return nullptr;
	}
	if (UWorld* World = GetWorld())
	{
		return World->IsActorInWorld(Pawn) ? Pawn : nullptr;
	}
	return Pawn;
}

void APlayerController::SetViewTarget(AActor* InViewTarget)
{
	ViewTarget = InViewTarget;
	if (UCameraComponent* Camera = FindCameraOnActor(InViewTarget))
	{
		ControlRotation = MakeControlRotationFromCamera(Camera);
	}
}

AActor* APlayerController::GetViewTarget() const
{
	if (!ViewTarget || !IsAliveObject(ViewTarget))
	{
		return nullptr;
	}
	if (UWorld* World = GetWorld())
	{
		return World->IsActorInWorld(ViewTarget) ? ViewTarget : nullptr;
	}
	return ViewTarget;
}

static UCameraComponent* FindCameraOnActor(AActor* Target)
{
	if (!Target)
	{
		return nullptr;
	}
	for (UActorComponent* Component : Target->GetComponents())
	{
		if (UCameraComponent* Camera = Cast<UCameraComponent>(Component))
		{
			return Camera;
		}
	}
	return nullptr;
}

UCameraComponent* APlayerController::ResolveViewCamera() const
{
	// ViewTarget에 카메라가 있으면 먼저 사용한다.
	if (UCameraComponent* Camera = FindCameraOnActor(GetViewTarget()))
	{
		return Camera;
	}

	// ViewTarget이 카메라 없는 Actor여도, Possess 중인 Pawn 카메라는 fallback으로 쓴다.
	if (APawn* CurrentPawn = GetPawn())
	{
		if (UCameraComponent* Camera = CurrentPawn->FindPawnCamera())
		{
			return Camera;
		}
	}

	return nullptr;
}
