// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

class APlayerCharacter;
class UItemBase;

UCLASS()
class ONLINE_RPG_API APickUpItem : public AActor, public IItemInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickUpItem();

	void InitializeItem(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);
	void PickUpItem(const APlayerCharacter* Taker);

	UFUNCTION(NetMulticast, Reliable)
	void InitializeDropItem(int32 ItemToDrop, const int32 Quantity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bIsConstructing = false;

public:
	UPROPERTY(VisibleAnywhere, Category = "ItemData")
	UStaticMeshComponent* InstanceMesh;

	UPROPERTY(EditDefaultsOnly, Category = "ItemData")
	UDataTable* InstanceItemDataTable;

	UPROPERTY(EditInstanceOnly, Category = "ItemData")
	FName InstanceItemID = TEXT("1");

	UPROPERTY(VisibleAnywhere, Category = "ItemData")
	UItemBase* InstanceItemData;

	UPROPERTY(ReplicatedUsing=OnRep_InstanceItemQuantity, EditInstanceOnly, Category = "ItemData")
	int32 InstanceItemQuantity;

	UPROPERTY(EditInstanceOnly, Category = "ItemData")
	FInteractionData InstanceItemInteractData;

	void SetItemBase(UItemBase* Base){InstanceItemData = Base;}
	void UpdateItemInteractionData();

	UFUNCTION(Server, Reliable)
	void RPC_Set_Quantity(int32 _Quantity);

	UFUNCTION()
	void OnRep_InstanceItemQuantity()
	{
		//UE_LOG(LogTemp, Log, TEXT("OnRep_InstanceItemQuantity ...%d "), InstanceItemQuantity);
		UE_LOG(LogTemp,Warning,TEXT("범인"))
		SetAllQuantity(InstanceItemQuantity);
	}

	//////////////////////////////////////////
	UPROPERTY(ReplicatedUsing = OnRep_SetItemBase)
	int32 ItemBase_Key = 1;

	UFUNCTION()
	void OnRep_SetItemBase();

	//////////////////////////////////////////
	void SetAllQuantity(int32 ChangeValue);
	
	// Server RPC �Լ� ����
	UFUNCTION(Server, Reliable)
	void ServerDestroyActor();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestSetOwner(APlayerController* NewOwner);

	// ���ø����̼��� ���� ������ ���� ����
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedOwner)
	int a = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedOwner)
	APawn* ReplicatedOwner;

	UFUNCTION()
	void OnRep_ReplicatedOwner();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact(APlayerCharacter* PlayerCharacter) override;

protected:
	virtual void BeginFocus() override;
	virtual void EndFocus() override;

	


	////#if WITH_EDITOR
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	////#endif
};
