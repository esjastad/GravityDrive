// ©, 2021, Erik Jastad. All Rights Reserved


#include "HISMManager.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AHISMManager::AHISMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AnchorCallCount = 0;
	Scene = CreateDefaultSubobject<USceneComponent>("MyRoot");
	RootComponent = Scene;
	//***********************************************************************************Move this HISM generation, EditAnywhere and ReadWrite into a BP****************************
	UHierarchicalInstancedStaticMeshComponent * NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Cube");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Wedge");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Corner");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("CubeAngle");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Angled");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	//Anchor HISM should always be last in the list, ***these are the white cubes that show good attach points when your mouse hovers over a placed segment!***
	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Anchor");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Window");
	NewHISM->SetupAttachment(RootComponent);
	ItemHISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Door");
	NewHISM->SetupAttachment(RootComponent);
	ItemHISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Stairs");
	NewHISM->SetupAttachment(RootComponent);
	ItemHISMList.Add(NewHISM);
}

// Called when the game starts or when spawned
void AHISMManager::BeginPlay()
{
	Super::BeginPlay();
	Material = UMaterialInstanceDynamic::Create(Wedge->GetMaterial(0), this);
	Material->SetVectorParameterValue(FName("Color"), FLinearColor::Red);

	
	HISMList[0]->SetStaticMesh(Cube);
	HISMList[1]->SetStaticMesh(Wedge);
	HISMList[2]->SetStaticMesh(Corner);
	HISMList[3]->SetStaticMesh(CubeAngle);
	HISMList[4]->SetStaticMesh(Angled);

	//AnchorHISM should always be last one
	AnchorMaterial = UMaterialInstanceDynamic::Create(AnchorMaterial, this);
	HISMList[HISMList.Num() - 1]->SetStaticMesh(Cube);
	HISMList[HISMList.Num() - 1]->SetMaterial(0, AnchorMatRef);
	HISMList[HISMList.Num() - 1]->bRenderCustomDepth = true;
	HISMList[HISMList.Num() - 1]->CustomDepthStencilValue = 5;

	//HISMList[HISMList.Num() - 1]->
	for (int i = 0; i < HISMList.Num() - 1; ++i)
	{
		HISMList[i]->CastShadow = false;
		HISMList[i]->SetMaterial(0, Material);
		HISMList[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	HISMList[HISMList.Num() - 1]->CastShadow = false;
	HISMList[HISMList.Num() - 1]->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemHISMList[0]->SetStaticMesh(Window);
	ItemHISMList[1]->SetStaticMesh(Door);
	ItemHISMList[2]->SetStaticMesh(Stairs);
	//ItemHISMList[0]->SetMaterial(0, Window->GetMaterial(0));
}

// Called every frame
void AHISMManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHISMManager::AddHullHISM(TMap<FVector, APieceData> * HISMData, FVector * Center)
{
	FTransform Tform;
	for (auto& Elem : *HISMData)
	{
		Tform = Elem.Value.Transform;
		Tform.SetLocation(*Center + Tform.GetLocation());
		switch (Elem.Value.StampType) {
		case AStampType::stampCube:
			HISMList[0]->AddInstance(Tform);
			break;
		case AStampType::stampWedge:
			HISMList[1]->AddInstance(Tform);
			break;
		case AStampType::stampCorner:
			HISMList[2]->AddInstance(Tform);
			break;
		case AStampType::stampCubeAngle:
			HISMList[3]->AddInstance(Tform);
			break;
		case AStampType::stampAngled:
			HISMList[4]->AddInstance(Tform);
			break;
		case AStampType::stampInvertedWedge:
			HISMList[1]->AddInstance(Tform);
			break;
		case AStampType::stampInvertedCorner:
			HISMList[2]->AddInstance(Tform);
			break;
		}
	}
}

void AHISMManager::Anchors(TArray<FTransform> * XForm, int CallsTillReset)
{
	if (AnchorCallCount != CallsTillReset || AnchorCallCount == 1)
	{
		HISMList[HISMList.Num() - 1]->ClearInstances();
		AnchorCallCount = CallsTillReset;
	}
	HISMList[HISMList.Num() - 1]->AddInstances(*XForm, false);
	
	GetWorldTimerManager().SetTimer(AnchorClearTimer, this, &AHISMManager::AnchorClear, 10.0f, false, 10.0f);
}

void AHISMManager::AnchorClear()
{
	HISMList[HISMList.Num() - 1]->ClearInstances();
}

void AHISMManager::AddItem(AItemType type, FTransform TForm)
{
	switch (type)
	{
	case AItemType::itemDoor:
	{

	}
	break;
	case AItemType::itemStairs:
	{

	}
	break;
	case AItemType::itemWindow:
	{
		ItemHISMList[0]->AddInstance(TForm);
	}
	break;
	}
}
