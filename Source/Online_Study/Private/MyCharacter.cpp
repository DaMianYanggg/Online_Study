// Fill out your copyright notice in the Description page of Project Settings.


#include "Online_Study/Public/MyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MyProjectile.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//初始化玩家生命值
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	//初始化投射物类
	ProjectileClass = AMyProjectile::StaticClass();
	//初始化射速
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(TEXT("/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin"));
	if (DefaultMesh.Succeeded())
	{
		this->GetMesh()->SetSkeletalMesh(DefaultMesh.Object);
		this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -95.0f));
		this->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}	
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//复制当前生命值。
	DOREPLIFETIME(AMyCharacter, CurrentHealth);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AMyCharacter::OnHealthUpdate()
{
	//客户端特定的功能
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//服务器特定的功能
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//在所有机器上都执行的函数。
	/*
		因任何因伤害或死亡而产生的特殊功能都应放在这里。
	*/
}

void AMyCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AMyCharacter::StopFire, FireRate, false);
		HandleFire();
	}
}
 
void AMyCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void AMyCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + ( GetControlRotation().Vector() * 100.0f ) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();
 
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;
 
	AMyProjectile* spawnedProjectile = GetWorld()->SpawnActor<AMyProjectile>(spawnLocation, spawnRotation, spawnParameters);
}


// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyCharacter::StartFire);
}

void AMyCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AMyCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

