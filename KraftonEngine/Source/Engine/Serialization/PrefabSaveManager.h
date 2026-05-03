#pragma once

#include <string>
#include "Core/CoreTypes.h"
#include "Math/Vector.h"

class AActor;
class UWorld;

class FPrefabSaveManager
{
public:
	static constexpr const wchar_t* PrefabExtension = L".Prefab";

	static bool SaveActorAsPrefab(AActor* Actor, const FString& PrefabName);
	static AActor* LoadPrefabActor(UWorld* World, const FString& PrefabName);
	static AActor* SpawnPrefab(UWorld* World, const FString& PrefabName, const FVector& SpawnLocation);
	static bool DoesPrefabExist(const FString& PrefabName);
	static TArray<FString> GetPrefabList();
};
