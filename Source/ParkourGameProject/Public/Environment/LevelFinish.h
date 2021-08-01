// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelFinish.generated.h"

class UBoxComponent;
class UUserWidget;

UCLASS()
class PARKOURGAMEPROJECT_API ALevelFinish : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ALevelFinish();

	// Components Setup

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Trigger;

	// Gameplay Variables

	UPROPERTY(EditAnyWhere, Category = "Gameplay")
	FName NextLevel;

	UPROPERTY(EditAnyWhere, Category = "Gameplay")
	float LevelChangeDelay;

	UPROPERTY(EditAnyWhere, Category = "Gameplay")
	float FadeDuration;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<UUserWidget> EndScreenWidgetClass;

	UPROPERTY()
	UUserWidget* EndScreenWidget;

protected:

	// Called when the game starts or when spawned

	virtual void BeginPlay() override;

	// Overlap Functions

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Gameplay Functions

	void ChangeLevel();

	// Timers

	FTimerHandle LevelChangeTimerHandle;


};
