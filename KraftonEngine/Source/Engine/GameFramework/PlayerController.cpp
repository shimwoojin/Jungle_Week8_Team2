#include "GameFramework/PlayerController.h"

#include "Component/ActorComponent.h"
#include "Component/CameraComponent.h"
#include "Component/ControllerInputComponent.h"
#include "Component/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/World.h"
#include "Serialization/Archive.h"

IMPLEMENT_CLASS(APlayerController, AActor)

static UCameraComponent* FindCameraOnActor(AActor* Target);

void APlayerController::Serialize(FArchive& Ar)
{
	AActor::Serialize(Ar);
	Ar << ControlRotation;
}

void APlayerController::InitDefaultComponents()
{
	if (!GetRootComponent())
	{
		if (UStaticMeshComponent* Root = AddComponent<UStaticMeshComponent>())
		{
			SetRootComponent(Root);
		}
	}
	if (!FindControllerInputComponent())
	{
		AddComponent<UControllerInputComponent>();
	}
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
	if (UCameraComponent* Camera = FindCameraOnActor(GetViewTarget()))
	{
		return Camera;
	}
	if (APawn* CurrentPawn = GetPawn())
	{
		if (UCameraComponent* Camera = CurrentPawn->FindPawnCamera())
		{
			return Camera;
		}
	}
	return nullptr;
}

UControllerInputComponent* APlayerController::FindControllerInputComponent() const
{
	for (UActorComponent* Component : GetComponents())
	{
		if (UControllerInputComponent* Input = Cast<UControllerInputComponent>(Component))
		{
			return Input;
		}
	}
	return nullptr;
}
