// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZone.generated.h"

class UBoxComponent;

UCLASS()
class PARKOURGAMEPROJECT_API AKillZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKillZone();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Time")
		float TimeDilation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Components")
		UBoxComponent* Trigger;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
