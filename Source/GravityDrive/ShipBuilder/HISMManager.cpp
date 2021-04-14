// ©, 2021, Erik Jastad. All Rights Reserved


#include "HISMManager.h"

// Sets default values
AHISMManager::AHISMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("MyRoot");
	RootComponent = Scene;

	UHierarchicalInstancedStaticMeshComponent * NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Cube");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Wedge");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Corner");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);
}

// Called when the game starts or when spawned
void AHISMManager::BeginPlay()
{
	Super::BeginPlay();
	Material = UMaterialInstanceDynamic::Create(Wedge->GetMaterial(0), this);
	Material->SetVectorParameterValue(FName("Color"), FLinearColor::Red);

	HISMList[0]->SetStaticMesh(Cube);
	HISMList[0]->SetMaterial(0, Material);
	HISMList[1]->SetStaticMesh(Wedge);
	HISMList[1]->SetMaterial(0, Material);
	HISMList[2]->SetStaticMesh(Corner);
	HISMList[2]->SetMaterial(0, Material);
	
}

// Called every frame
void AHISMManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHISMManager::AddHullHISM(TMap<FVector, APieceData> * HISMData)
{
	for (auto& Elem : *HISMData)
	{
		switch (Elem.Value.StampType) {
		case AStampType::stampCube:
			HISMList[0]->AddInstance(Elem.Value.Transform);
			break;
		case AStampType::stampWedge:
			HISMList[1]->AddInstance(Elem.Value.Transform);
			break;
		case AStampType::stampCorner:
			HISMList[2]->AddInstance(Elem.Value.Transform);
			break;
		}
	}
}

