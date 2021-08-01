// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuardRobotInterface.h"
#include "GuardRobot.generated.h"

// Forward Declerations
class UStaticMeshComponent;
class USceneComponent;
class UParticleSystemComponent;
class USoundWave;
class UCameraShakeBase;

UCLASS()
class PARKOURGAMEPROJECT_API AGuardRobot : public AActor, public IGuardRobotInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties

	AGuardRobot();

	// Components Setup

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RobotMesh;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* NewVisionField;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* DestructEffect1;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* DestructEffect2;

	// Interface Functions

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnLookedAt(AActor* Player);
	virtual void OnLookedAt_Implementation(AActor* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnStopLookedAt();
	virtual void OnStopLookedAt_Implementation();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Overlap

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Gameplay Variables

	UPROPERTY(BlueprintReadOnly)
	bool bDisabled;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Time")
	float TimeDilation;

	UPROPERTY(BlueprintReadOnly)
	bool bSlowMotion;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	USoundWave* DestructSound1;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	USoundWave* DestructSound2;

	UPROPERTY(EditAnyWhere, Category = "Time")
	float PatrolDelay;

	UPROPERTY(EditAnyWhere, Category = "Time")
	float BustedDelay;

	// Gameplay Functions

	UFUNCTION()
	void RotateToPlayer();

	// CameraShake

	UPROPERTY(EditAnyWhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> ExplodeCamShake;

private:

	// Timers

	FTimerHandle SlowMotionTimerHandle;

	FTimerHandle LookUpTimerHandle;

	FTimerHandle LookDownTimerHandle;

	FTimerHandle DestroyDelayTimerHandle;

	FTimerHandle LookAtPlayerTimerHandle;

	// Gameplay Variables

	bool bSeesPlayer;

	AActor* PlayerRef;

	bool bGameOver;

	// Gameplay Functions

	void StartSlowmotion();
	void EndGame();
	void StopSlowmotion();

	void LookUp();
	void LookDown();

	void Destruct();


};
