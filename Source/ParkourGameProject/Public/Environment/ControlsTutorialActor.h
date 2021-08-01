// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlsTutorialActor.generated.h"

class USceneComponent;
class UWidgetComponent;
class UBoxComponent;
class USoundWave;

UCLASS()
class PARKOURGAMEPROJECT_API AControlsTutorialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControlsTutorialActor();

	// Components Setup

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* ControlsWidget;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Trigger;

	// Controls Widget Settings

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Widget")
	FText Input;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Widget")
	FColor BackGroundColor;

	// Timer Functions

	void OnLifeTimerEnded();

	// Timer Variables

	UPROPERTY(EditAnyWhere, Category = "Timer")
	float LifeTime;

	// Sound Variables

	UPROPERTY(EditAnyWhere, Category = "Sound")
	USoundWave* NotificationSound;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	float NotificationSoundStartTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Overlap

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bOverlapped;

	// Timers

	FTimerHandle LifeTimerHandle;

};
