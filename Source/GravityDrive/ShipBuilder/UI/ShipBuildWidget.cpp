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

void UShipBuildWidget::SetSegType(int type)
{
	SegType = (type > 0) ? (type > 1) ? SegmentType::sphere : SegmentType::cylinder : SegmentType::cube;
	UpdateBuildState();
}
