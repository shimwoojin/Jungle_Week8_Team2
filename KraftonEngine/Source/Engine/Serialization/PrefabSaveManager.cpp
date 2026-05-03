#include "PrefabSaveManager.h"
#include "SceneSaveManager.h"
#include "SimpleJSON/json.hpp"
#include "Platform/Paths.h"
#include "GameFramework/AActor.h"
#include "GameFramework/World.h"
#include <iostream>
#include <fstream>

bool FPrefabSaveManager::SaveActorAsPrefab(AActor* Actor, const FString& PrefabName)
{
	if (!Actor || PrefabName.empty())
	{
		std::cerr << "[Prefab] SaveActorAsPrefab failed: Actor is null or PrefabName is empty\n";
		return false;
	}

	using namespace json;
	JSON Root = json::Object();
	Root["Version"] = 1;
	Root["Type"] = "ActorPrefab";
	Root["Name"] = PrefabName.c_str();

	JSON RootActor = FSceneSaveManager::SerializeActor(Actor);
	Root["RootActor"] = RootActor;

	std::wstring PrefabDir = FPaths::PrefabDir();
	FPaths::CreateDir(PrefabDir);
	std::filesystem::path FileDestination = std::filesystem::path(PrefabDir) / (FPaths::ToWide(PrefabName) + PrefabExtension);

	std::ofstream File(FileDestination);
	if (File.is_open())
	{
		File << Root.dump();
		File.flush();
		File.close();
		std::cout << "[Prefab] Saved: " << FPaths::ToUtf8(FileDestination.wstring()) << "\n";
		return true;
	}

	std::cerr << "[Prefab] SaveActorAsPrefab failed: Could not open file for writing\n";
	return false;
}

AActor* FPrefabSaveManager::LoadPrefabActor(UWorld* World, const FString& PrefabName)
{
	if (!World || PrefabName.empty()) return nullptr;

	std::wstring PrefabDir = FPaths::PrefabDir();
	std::filesystem::path FileDestination = std::filesystem::path(PrefabDir) / (FPaths::ToWide(PrefabName) + PrefabExtension);

	if (!std::filesystem::exists(FileDestination))
	{
		std::cerr << "[Prefab] Load failed: file not found - " << FPaths::ToUtf8(FileDestination.wstring()) << "\n";
		return nullptr;
	}

	std::ifstream File(FileDestination);
	if (!File.is_open()) return nullptr;

	std::string FileContent((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());
	json::JSON root = json::JSON::Load(FileContent);

	if (!root.hasKey("Type") || root["Type"].ToString() != "ActorPrefab")
	{
		std::cerr << "[Prefab] Load failed: Invalid prefab type\n";
		return nullptr;
	}

	if (!root.hasKey("RootActor"))
	{
		std::cerr << "[Prefab] Load failed: Missing RootActor\n";
		return nullptr;
	}

	json::JSON& RootActorJson = root["RootActor"];
	AActor* Actor = FSceneSaveManager::DeserializeActor(World, RootActorJson);
	
	return Actor;
}

AActor* FPrefabSaveManager::SpawnPrefab(UWorld* World, const FString& PrefabName, const FVector& SpawnLocation)
{
	AActor* Actor = LoadPrefabActor(World, PrefabName);
	if (!Actor) return nullptr;

	Actor->SetActorLocation(SpawnLocation);

	World->RemoveActorToOctree(Actor);
	World->InsertActorToOctree(Actor);
	
	if (World->HasBegunPlay() && !Actor->HasActorBegunPlay() && !Actor->IsPooledActorInactive())
	{
		Actor->BeginPlay();
	}

	std::cout << "[Prefab] Spawned: " << PrefabName << " at (" << SpawnLocation.X << ", " << SpawnLocation.Y << ", " << SpawnLocation.Z << ")\n";

	return Actor;
}

bool FPrefabSaveManager::DoesPrefabExist(const FString& PrefabName)
{
	std::wstring PrefabDir = FPaths::PrefabDir();
	std::filesystem::path FileDestination = std::filesystem::path(PrefabDir) / (FPaths::ToWide(PrefabName) + PrefabExtension);
	return std::filesystem::exists(FileDestination);
}

TArray<FString> FPrefabSaveManager::GetPrefabList()
{
	TArray<FString> Result;
	std::wstring PrefabDir = FPaths::PrefabDir();
	if (!std::filesystem::exists(PrefabDir))
	{
		return Result;
	}

	for (auto& Entry : std::filesystem::directory_iterator(PrefabDir))
	{
		if (Entry.is_regular_file() && Entry.path().extension() == PrefabExtension)
		{
			Result.push_back(FPaths::ToUtf8(Entry.path().stem().wstring()));
		}
	}
	return Result;
}
