// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackgroundMusicActor.generated.h"

class UAudioComponent;

UCLASS()
class PARKOURGAMEPROJECT_API ABackgroundMusicActor : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ABackgroundMusicActor();

	UPROPERTY(VisibleAnyWhere, Category = "Components")
	UAudioComponent* MusicComp;

	UPROPERTY(EditAnyWhere, Category = "Sound Settings")
	float FadeInDuration;

	UPROPERTY(EditAnyWhere, Category = "Sound Settings")
	float SoundStartTime;

	void ChangePitch(float newPitch);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
};
