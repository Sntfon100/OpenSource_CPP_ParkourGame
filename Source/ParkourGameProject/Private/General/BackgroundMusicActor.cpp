// Fill out your copyright notice in the Description page of Project Settings.


#include "General/BackgroundMusicActor.h"
#include "Components/AudioComponent.h"

// Sets default values
ABackgroundMusicActor::ABackgroundMusicActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MusicComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComp"));
	RootComponent = MusicComp;

	MusicComp->SetVolumeMultiplier(0.5f);

	FadeInDuration = 2.0f;

}

// Called when the game starts or when spawned
void ABackgroundMusicActor::BeginPlay()
{
	Super::BeginPlay();
	
	MusicComp->FadeIn(FadeInDuration, MusicComp->VolumeMultiplier, SoundStartTime);

}

void ABackgroundMusicActor::ChangePitch(float newPitch)
{
	MusicComp->SetPitchMultiplier(newPitch);
}


