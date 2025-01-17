// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryMainMenu.h"

#include "ItemBase.h"
#include "ItemC.h"
#include "ItemDragDrop.h"

void UInventoryMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
}



bool UInventoryMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (!PlayerCharacter)
	{
		PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	}
	const UItemDragDrop* ItemDragDrop = Cast<UItemDragDrop>(InOperation);
	
	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem,ItemDragDrop->SourceItem->BaseItemQuantity);
		return true;
	}

	return  false;
	
	//return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
