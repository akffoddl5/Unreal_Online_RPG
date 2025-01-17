// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemTooltip.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"


class AInventoryHUD;
class UInventoryComponent;
class APlayerCharacter;
class UTextBlock;
class UWrapBox;
class UItemSlot;
/**
 * 
 */
UCLASS()
class ONLINE_RPG_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryContents;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;
	UPROPERTY()
	UInventoryComponent* Inventory;
	UPROPERTY(EditDefaultsOnly, Category = "InventorySlot")
	TSubclassOf<UItemSlot> InventorySlotClass;

	bool ActiveInitialize = false;
	void InitializePanel(APlayerCharacter* Player);
	APlayerCharacter* GetCharacter() const {return PlayerCharacter;}
	void RefreshInventory();
protected:
	void WriteInfoText() const;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
};
