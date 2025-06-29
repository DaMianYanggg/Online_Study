﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online_Study/Online_StudyCharacter.h"
#include "MyCharacter.generated.h"

UCLASS()
class ONLINE_STUDY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();
	/** 属性复制 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 玩家的最大生命值。这是玩家的最高生命值，也是出生时的生命值。*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	/** 玩家的当前生命值。降到0就表示死亡。*/
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	/** RepNotify，用于同步对当前生命值所做的更改。*/
	UFUNCTION()
	void OnRep_CurrentHealth();
	/** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify*/
	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category="Gameplay|Projectile")
	TSubclassOf<class AMyProjectile> ProjectileClass;
 
	/** 射击之间的延迟，单位为秒。用于控制测试发射物的射击速度，还可防止服务器函数的溢出导致将SpawnProjectile直接绑定至输入。*/
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float FireRate;
 
	/** 若为true，则正在发射投射物。*/
	bool bIsFiringWeapon;
 
	/** 用于启动武器射击的函数。*/
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StartFire();
 
	/** 用于结束武器射击的函数。一旦调用这段代码，玩家可再次使用StartFire。*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();
 
	/** 用于生成投射物的服务器函数。*/
	UFUNCTION(Server, Reliable)
	void HandleFire();
 
	/** 定时器句柄，用于提供生成间隔时间内的射速延迟。*/
	FTimerHandle FiringTimer;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 最大生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** 当前生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** 当前生命值的存值函数。将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。仅在服务器上调用。*/
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetCurrentHealth(float healthValue);

	/** 承受伤害的事件。从APawn覆盖。*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage( float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) override;
};
