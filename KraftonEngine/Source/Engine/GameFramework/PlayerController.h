#pragma once
#include "GameFramework/AActor.h"

class APawn;
class UCameraComponent;
class FArchive;

enum class EControllerMovementFrame : int32
{
	World = 0,
	Camera = 1,
};

enum class EControllerLookMode : int32
{
	Auto = 0,
	CameraOnly = 1,
	PawnYawPawnPitch = 2,

	PawnYawCameraPitch = PawnYawPawnPitch,
};

class APlayerController : public AActor
{
public:
	DECLARE_CLASS(APlayerController, AActor)

	APlayerController() = default;
	~APlayerController() override = default;

	void Serialize(FArchive& Ar) override;
	void GetEditableProperties(TArray<FPropertyDescriptor>& OutProps) override;
	void PostEditProperty(const char* PropertyName) override;
	void InitDefaultComponents() override;

	void EndPlay() override;

	void Possess(APawn* InPawn);
	void UnPossess();

	APawn* GetPawn() const;

	void SetViewTarget(AActor* InViewTarget);
	AActor* GetViewTarget() const;

	UCameraComponent* ResolveViewCamera() const;

	FRotator GetControlRotation() const { return ControlRotation; }
	void SetControlRotation(const FRotator& InRotation) { ControlRotation = InRotation; }

	EControllerMovementFrame GetMovementFrame() const { return static_cast<EControllerMovementFrame>(MovementFrame); }
	void SetMovementFrame(EControllerMovementFrame InFrame);

	EControllerLookMode GetLookMode() const { return static_cast<EControllerLookMode>(LookMode); }
	void SetLookMode(EControllerLookMode InMode);

private:
	APawn* Pawn = nullptr;
	AActor* ViewTarget = nullptr;
	FRotator ControlRotation;
	int32 MovementFrame = static_cast<int32>(EControllerMovementFrame::Camera);
	int32 LookMode = static_cast<int32>(EControllerLookMode::Auto);
};
