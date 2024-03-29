// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipBuildWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Called when widget is instantiated
void UShipBuildWidget::OnInitialized()
{
	Super::OnInitialized();

}

void UShipBuildWidget::SetShipReference(AShipManager * NewRef)
{
	ShipManagerRef = NewRef;
}

void UShipBuildWidget::UpdateBuildState()
{
	ShipManagerRef->UpdateSegType(&SegType);
}

// Because this function is Blueprint Callable, the Enums are not BP Enums so an int is used, converted to byte selection of ENUM
void UShipBuildWidget::SetSegType(SegmentType newType)
{
	SegType = newType;// SegmentType(EByte); //(type > 0) ? (type > 1) ? SegmentType::sphere : SegmentType::cylinder : SegmentType::cube;
	UpdateBuildState();
}

// Because this function is Blueprint Callable, the Enums are not BP Enums so an int is used, converted to byte selection of ENUM
void UShipBuildWidget::SetBuildMode(BuildMode newMode)
{	
	ShipManagerRef->UpdateBuildMode(newMode);
}

// Send mirror mode selection update to ship manager, 1 for on, -1 for off.
void UShipBuildWidget::UpdateMirrorMode(FVector newMode)
{
	ShipManagerRef->UpdateMirrorMode(newMode);
}

void UShipBuildWidget::SetItemType(AItemType type)
{
	ShipManagerRef->UpdateItemMode(type);
}