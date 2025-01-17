// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDog.h"
#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.h"
#include "CMSpawnManager.h"
#include "EnemyAIController.h"
#include "AItemManager.h"
#include "Network_Manager_R.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

// Sets default values
AEnemyDog::AEnemyDog()
{
	SetReplicateMovement(true);
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// 리플리케이트된 프로퍼티
void AEnemyDog::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//현재 체력 리플리케이트
	DOREPLIFETIME(AEnemyDog, bIsCollision);
	DOREPLIFETIME(AEnemyDog, IsDead);
	DOREPLIFETIME(AEnemyDog, IsHit);
	DOREPLIFETIME(AEnemyDog, IsAttack);


}

// Called when the game starts or when spawned
void AEnemyDog::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	//RangeCheck();

	SpawnLocation = GetActorLocation() + FVector(100.0f, 100.0f, 0.0f);

	/*FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyDog::HandleAttack, 2.f, true);*/
}

// Called every frame
void AEnemyDog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

// Called to bind functionality to input
void AEnemyDog::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyDog::Attack_Implementation()
{
	//float TimeSinceBegin = GetWorld()->GetTimeSeconds();
	////UE_LOG(LogTemp, Warning, TEXT("%f/ %f /%f"), AttackDelay, AttackTime, TimeSinceBegin);

	//if (AttackDelay + AttackTime < TimeSinceBegin)
	//{
	//	SpawnProjectile();
	//	AttackTime = TimeSinceBegin;
	//}
	//서버 전용 함수 기능
	//if (GetLocalRole() == ROLE_Authority)
	//{
	//	FTimerHandle TimerHandle;
	//	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyDog::EnemyAttack, 1.4f, true);
	//	//EnemyAttack();
	//}
	if (GetLocalRole() == ROLE_Authority) {

		IsAttack = true;
	}
}


void AEnemyDog::HitOff()
{
	IsHit = false;
}

float AEnemyDog::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead) return 0;
	SetFreeze(true);
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	AEnemyAIController* OwnerController = Cast<AEnemyAIController>(this->GetController());
	if (OwnerController)
		OwnerController->SetPlayer(Cast<APawn>(DamageCauser));

	if (Health - ActualDamage <= 0)
	{
		Dead();
		return ActualDamage;
	}

	Health -= ActualDamage;
	////UE_LOG(LogTemp, Display, TEXT("Actor took damage: %f Heath = %f"), ActualDamage, Health);
	return ActualDamage;
}
void AEnemyDog::SetFreeze(bool _tf)
{
	AEnemyAIController* OwnerController = Cast<AEnemyAIController>(this->GetController());

	OwnerController->GetBlackboardComponent()->SetValueAsBool("Freeze", _tf);

}


void AEnemyDog::SpawnDebugSphere(FVector Location, float Radius)
{
	DrawDebugSphere(
		GetWorld(),
		Location,
		Radius,
		20,
		FColor::Red,
		false,
		2, // 스피어를 유지할 시간(초)
		0,
		1
	);
}

void AEnemyDog::SpawnProjectile()
{
	TSubclassOf<AEnemyProjectile> ActorClassToSpawn = AEnemyProjectile::StaticClass();

	FVector ProjectileSpawnLocation = this->GetActorLocation();
	FRotator ProjectileSpawnRotation = this->GetActorRotation();

	FVector SpawnScale = this->GetActorScale() * 2;

	// FActorSpawnParameters를 초기화합니다.
	FActorSpawnParameters SpawnParams;

	// 위치, 회전, 스케일을 포함한 FTransform 객체를 생성합니다.
	FTransform SpawnTransform(ProjectileSpawnRotation, ProjectileSpawnLocation, SpawnScale);


	AActor* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(AttackProjectile, SpawnTransform, SpawnParams);
	//AActor* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(AttackProjectile, ProjectileSpawnLocation, ProjectileSpawnRotation);
}

bool AEnemyDog::RangeCheck(int _AttackNum)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	float TargetDis = FVector::Dist(this->GetActorLocation(), PlayerPawn->GetActorLocation());

	if (_AttackNum == 1)
	{
		if (AttackRange1 >= TargetDis)
		{
			return true;
		}
		return false;
	}
	else if (_AttackNum == 2)
	{
		if (AttackRange2 >= TargetDis)
		{
			return true;
		}
		return false;
	}
	return false;
}

float AEnemyDog::GetAttackRange(int _AttackNum)
{
	if (_AttackNum == 1)
		return AttackRange1;
	else if (_AttackNum == 2)
		return AttackRange2;
	return 0;
}
void AEnemyDog::OnRep_IsAttack()
{
	if (IsAttack) {

		UE_LOG(LogTemp, Display, TEXT("OnRep_IsAttack... true..."));

	}
	else {
		UE_LOG(LogTemp, Display, TEXT("OnRep_IsAttack... false..."));

	}
}
void AEnemyDog::Dead()
{
	if (IsDead) return;
	UWorld* World = GetWorld();
	//UE_LOG(LogTemp, Warning, TEXT("죽음"));
	if (!World)
	{
		return;
	}
	else if (SpawnManager == nullptr)
	{
		SpawnManager = Cast<ACMSpawnManager>(UGameplayStatics::GetActorOfClass(World, ACMSpawnManager::StaticClass()));
		if (SpawnManager == nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("스폰못찾음"));
			return;
		}

	}


	if (SpawnManager)
	{
		SpawnManager->SetSpawnEnemyDog(SpawnLocation, 10.0f);
		//UE_LOG(LogTemp, Warning, TEXT("스폰함수 시작"));
	}
	Health = 0;
	IsDead = true;
	//FRotator MyRotator(0.0f, 0.0f, 190.0f);
	//this->AddActorLocalRotation(MyRotator);
	AEnemyAIController* OwnerController = Cast<AEnemyAIController>(this->GetController());
	//콜리전 끔
	SetIsCollision(true);
	if (OwnerController)
	{
		OwnerController->Dead();
	}
	else
	{
		////UE_LOG(LogTemp, Warning, TEXT("AEnemyAIController Is Nullptr"));
	}
	//	ItemManager& ItemManager = ItemManager::Get();

	ItemManagerInstance = Cast<UNetwork_Manager_R>(GetGameInstance())->GetItemManager();
	const FVector UpvVector{ 0,0,1 };
	const FVector SpawnLoc{ GetActorLocation() + UpvVector * 10.0f };
	const FTransform SpawnTransform(GetActorRotation(), SpawnLoc);

	float RandomValue = FMath::FRandRange(0.0f, 1.0f);

	if (RandomValue > 0.3f)
	{
		int RandomMoney = FMath::RandRange(80, 1000);
		while (RandomMoney > 100)
		{
			UItemBase* Base = ItemManagerInstance->MakeItemBaseByKey(this, 8, 7);
			ItemManagerInstance->SpawnItem(this, Base, SpawnTransform, 100);
			RandomMoney -= 100;
		}
		UItemBase* Base = ItemManagerInstance->MakeItemBaseByKey(this, 8, 7);
		ItemManagerInstance->SpawnItem(this, Base, SpawnTransform, RandomMoney);
	}

	RandomValue = FMath::FRandRange(0.0f, 1.0f);

	if (RandomValue > 0.6f)
	{
		int RandomPotion = FMath::RandRange(1, 5);

		UItemBase* Base = ItemManagerInstance->MakeItemBaseByKey(this, 4, 7);
		ItemManagerInstance->SpawnItem(this, Base, SpawnTransform, RandomPotion);
	}

	RandomValue = FMath::FRandRange(0.0f, 1.0f);

	if (RandomValue > 0.5f)
	{

		UItemBase* Base = ItemManagerInstance->MakeItemBaseByKey(this, 1, 7);
		ItemManagerInstance->SpawnItem(this, Base, SpawnTransform, 1);
	}


	if (bIsBoss)
	{
		UItemBase* Base = ItemManagerInstance->MakeItemBaseByKey(this, 7, 7);
		ItemManagerInstance->SpawnItem(this, Base, SpawnTransform, 1);
	}

	this->SetLifeSpan(5.0f);
}

void AEnemyDog::SetIsCollision(bool IsCollision)
{
	//서버 전용 함수 기능
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsCollision = IsCollision;
		OnIsCollisionUpdate();
	}
}

void AEnemyDog::OnRep_IsCollision()
{
	OnIsCollisionUpdate();
}

void AEnemyDog::OnIsCollisionUpdate()
{
	//클라이언트 전용 함수 기능
	if (IsLocallyControlled()) {

	}

	//서버 전용 함수 기능
	if (GetLocalRole() == ROLE_Authority)
	{

	}

	if (bIsCollision) {
		//콜리전 끔
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 죽음 상태에서는 무브먼트 끔
		GetCharacterMovement()->DisableMovement();
	}

}

void AEnemyDog::SpawnSelf()
{

}

ACMSpawnManager* FindSpawnManager(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	ACMSpawnManager* SpawnManager = Cast<ACMSpawnManager>(UGameplayStatics::GetActorOfClass(World, ACMSpawnManager::StaticClass()));

	return SpawnManager;
}

void AEnemyDog::HandleAttack()
{
	//서버 전용 함수 기능
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT(" Attking2"));



		//FTimerHandle TimerHandle;
		//GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyDog::EnemyAttack, 1.4f, false);
		EnemyAttack();
	}

}

void AEnemyDog::EnemyAttack()
{
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	//콜리전 크기
	FVector CollisionExtent = GetSimpleCollisionCylinderExtent();

	FVector CapsuleSize = FVector(EnemyAttackRadius, EnemyAttackRadius, EnemyAttackHeight) * GetActorScale().X;

	FCollisionShape AttackShape = FCollisionShape::MakeCapsule(CapsuleSize);

	FVector AttackLocation = (Location + FVector(0, 0, -CollisionExtent.Z + EnemyAttackEffectOffsetZ)) + Rotation.Vector() * EnemyAttackDistance;
	TArray<FHitResult> Hits;
	FCollisionQueryParams params;

	params.AddIgnoredActor(this);
	params.AddIgnoredActor(GetOwner());

	//UParticleSystemComponent* ParticleSystemComponent = nullptr;
	SpawnEmitterAtLocation_Multi(this, EnemyAttackPaticles, AttackLocation, GetActorRotation() + FRotator(-90, 0, 0), FVector(EnemyAttackEffectScale * 0.7f, EnemyAttackEffectScale, EnemyAttackEffectScale) * GetActorScale().X);
	//SpawnEmitterAtLocation_SetTimer_Multi(this, UpperSlashFinishPaticles, UpperSlashEffectDurationSec, AttackLocation, GetActorRotation(), FVector(UpperSlashEffectFinishScale));

	bool bSuccess = GetWorld()->SweepMultiByChannel(
		Hits,
		AttackLocation,
		AttackLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		AttackShape,
		params
	);

	if (bSuccess)
	{
		FVector ShotDirection = -Rotation.Vector();

		for (int i = 0; i < Hits.Num(); i++) {
			AActor* HitActor = Hits[i].GetActor();
			if (HitActor != nullptr && GetController()->LineOfSightTo(HitActor))
			{
				//SpawnDebugSphere(Hit.ImpactPoint, 30);


				//피격 이펙트 생성
				//UGameplayStatics::SpawnEmitterAtLocation(this, ShootHitPaticles, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(ShootHitEffectScale));

				FPointDamageEvent DamageEvent(EnemyAttackDamage, Hits[i], ShotDirection, nullptr);
				HitActor->TakeDamage(EnemyAttackDamage, DamageEvent, GetController(), this);
				if (Cast<APlayerCharacter>(HitActor)) {
					APlayerCharacter* __Tmp = Cast<APlayerCharacter>(HitActor);
					
					if (__Tmp->GetCurrentHealth() <= 0.1f) {
						AEnemyAIController* OwnerController = Cast<AEnemyAIController>(this->GetController());
						if (OwnerController) {
							OwnerController->bIsChangeCool = false;
							OwnerController->SetPlayer(nullptr);
						}
					}
				}
				else if (Cast<AEnemyDog>(HitActor)) {
					if (Cast<AEnemyDog>(HitActor)->Health <= 0.1f) {
						AEnemyAIController* OwnerController = Cast<AEnemyAIController>(this->GetController());
						if (OwnerController) {
							OwnerController->bIsChangeCool = false;
							OwnerController->SetPlayer(nullptr);
						}
					}
				}
			}
		}
	}

	//SpawnDebugCapsule(AttackLocation, CapsuleSize);
}

void AEnemyDog::SpawnEmitterAtLocation_Multi_Implementation(const UObject* WorldContextObject, UParticleSystem* Particle, FVector Location, FRotator Rotation, FVector Scale)
{
	////UE_LOG(LogTemp, Warning, TEXT("SpawnEmitterAtLocation"));
	//피격 이펙트 생성
	UParticleSystemComponent* ParticleSystemComponent;
	ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject, Particle, Location, Rotation, Scale);

}

void AEnemyDog::SpawnDebugCapsule(FVector Location, FVector CapsuleSize, FColor Color) {
	////UE_LOG(LogTemp, Warning, TEXT("DrawDebugCapsule"));
	DrawDebugCapsule(
		GetWorld(),
		Location,
		CapsuleSize.Z / 2,
		CapsuleSize.X,
		FQuat::Identity,
		Color,
		false,
		2,
		0,
		1
	);
}