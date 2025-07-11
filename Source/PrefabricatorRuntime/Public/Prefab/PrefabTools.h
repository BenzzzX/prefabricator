//$ Copyright 2015-24, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class APrefabActor;
class UPrefabricatorAsset;
struct FPrefabricatorActorData;
class UPrefabricatorProperty;
struct FRandomStream;

struct PREFABRICATORRUNTIME_API FPrefabLoadSettings {
	bool bUnregisterComponentsBeforeLoading = true;
    bool bDestroyUnusedActors = true;
	bool bRandomizeNestedSeed = false;
	bool bSynchronousBuild = true;
	bool bCanLoadFromCachedTemplate = true;
	bool bCanSaveToCachedTemplate = true;
	const FRandomStream* Random = nullptr;
};

struct PREFABRICATORRUNTIME_API FPrefabInstanceTemplateInfo {
	TWeakObjectPtr<AActor> TemplatePtr;
	FGuid PrefabLastUpdateId;
};

class PREFABRICATORRUNTIME_API FPrefabInstanceTemplates {
public:
	void RegisterTemplate(const FGuid& InPrefabItemId, FGuid InPrefabLastUpdateId, AActor* InActor);
	AActor* GetTemplate(const FGuid& InPrefabItemId, FGuid InPrefabLastUpdateId);
#if WITH_EDITOR
    void OnObjectPropertyChanged(UObject* ModifiedObject, struct FPropertyChangedEvent&);
#endif

private:
	TMap<FGuid, FPrefabInstanceTemplateInfo> PrefabItemTemplates;
    TSet<TWeakObjectPtr<AActor>> RegisteredActors;
};

class PREFABRICATORRUNTIME_API FGlobalPrefabInstanceTemplates {
public:
	FORCEINLINE static FPrefabInstanceTemplates* Get() { return Instance; }

	static void _CreateSingleton();
	static void _ReleaseSingleton();

private:
	static FPrefabInstanceTemplates* Instance;
};

class FPrefabActorLookup {
public:
	void Register(const FString& InActorPath, const FGuid& InPrefabItemId);
	void Register(AActor* InActor, const FGuid& InPrefabItemId);
	bool GetPrefabItemId(const FString& InObjectPath, FGuid& OutCrossRefPrefabItem) const;

private:
	TMap<FString, FGuid> ActorPathToItemId;
};

class PREFABRICATORRUNTIME_API FPrefabTools {
public:
	static bool CanCreatePrefab();
	static void CreatePrefab();
	static APrefabActor* CreatePrefabFromActors(const TArray<AActor*>& Actors);
	static void AssignAssetUserData(AActor* InActor, const FGuid& InItemID, APrefabActor* Prefab);

	static void SaveStateToPrefabAsset(APrefabActor* PrefabActor);
	static void LoadStateFromPrefabAsset(APrefabActor* PrefabActor, const FPrefabLoadSettings& InSettings = FPrefabLoadSettings());

	static void FixupCrossReferences(const TArray<UPrefabricatorProperty*>& PrefabProperties, UObject* ObjToWrite, TMap<FGuid, AActor*>& PrefabItemToActorMap);

	static void UnlinkAndDestroyPrefabActor(APrefabActor* PrefabActor);
	static void GetActorChildren(AActor* InParent, TArray<AActor*>& OutChildren);

	static void GetActorChildrenRecursive(AActor* InParent, TArray<AActor*>& OutChildren);
	static void BuildHierarchicalActorData(AActor* InParent, APrefabActor* PrefabActor);

	static FBox GetPrefabBounds(AActor* PrefabActor, bool bNonColliding = true);
	static bool ShouldIgnorePropertySerialization(const FName& PropertyName);
	static bool ShouldForcePropertySerialization(const FName& PropertyName);

	static void ParentActors(AActor* ParentActor, AActor* ChildActor);
	static void SelectPrefabActor(AActor* PrefabActor);
	static void GetSelectedActors(TArray<AActor*>& OutActors);
	static int GetNumSelectedActors();
	static UPrefabricatorAsset* CreatePrefabAsset();
	static int32 GetRandomSeed(const FRandomStream& Random);

	static void IterateChildrenRecursive(APrefabActor* Actor, TFunction<void(AActor*)> Visit);

private:
	static void SaveActorState(AActor* InActor, APrefabActor* PrefabActor, const FPrefabActorLookup& CrossReferences, FPrefabricatorActorData& OutActorData);
	static void LoadActorState(AActor* InActor, const FPrefabricatorActorData& InActorData, const FPrefabLoadSettings& InSettings);
	static void BuildActorDataRecursive(AActor* InParent, APrefabActor* PrefabActor, TArray<FPrefabricatorActorData>& OutActorData, FPrefabActorLookup& CrossReferences, int32 CurrentDepth);

};

class PREFABRICATORRUNTIME_API FPrefabVersionControl {
public:
	static void UpgradeToLatestVersion(UPrefabricatorAsset* Prefab);

private:
	static void UpgradeFromVersion_InitialVersion(UPrefabricatorAsset* Prefab);
	static void UpgradeFromVersion_AddedSoftReferences(UPrefabricatorAsset* Prefab);
	static void UpgradeFromVersion_AddedSoftReferencesPrefabFix(UPrefabricatorAsset* Prefab);

private:
	static void RefreshReferenceList(UPrefabricatorAsset* Prefab);
};

