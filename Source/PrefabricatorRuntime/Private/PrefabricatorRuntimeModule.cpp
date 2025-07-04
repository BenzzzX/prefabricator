//$ Copyright 2015-24, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "PrefabricatorRuntimeModule.h"

#include "Prefab/PrefabTools.h"
#include "Utils/PrefabricatorService.h"

class FPrefabricatorRuntime : public IPrefabricatorRuntime
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

    FDelegateHandle ObjectPropertyChangedHandle;
};

IMPLEMENT_MODULE(FPrefabricatorRuntime, PrefabricatorRuntime)

void FPrefabricatorRuntime::StartupModule()
{
	// Set the runtime prefabricator service
	// Set this only if it is null (the editor might have set the editor service, and if so, we won't override it)
	if (!FPrefabricatorService::Get().IsValid()) {
		FPrefabricatorService::Set(MakeShareable(new FPrefabricatorRuntimeService));
	}

	FGlobalPrefabInstanceTemplates::_CreateSingleton();

    ObjectPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(FGlobalPrefabInstanceTemplates::Get(), &FPrefabInstanceTemplates::OnObjectPropertyChanged);
}


void FPrefabricatorRuntime::ShutdownModule()
{
	// Clear the service object
	FPrefabricatorService::Set(nullptr);

    FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ObjectPropertyChangedHandle);

	FGlobalPrefabInstanceTemplates::_ReleaseSingleton();
}

