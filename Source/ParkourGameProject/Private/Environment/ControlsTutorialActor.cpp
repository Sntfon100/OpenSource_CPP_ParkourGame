// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/ControlsTutorialActor.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Public/TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Sets default values
AControlsTutorialActor::AControlsTutorialActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ControlsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ControlsWidget"));
	ControlsWidget->SetupAttachment(RootComponent);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);

	SetActorHiddenInGame(true);

	LifeTime = 2.0f;

	NotificationSoundStartTime = 0.0f;

	bOverlapped = false;

}


// Called when the game starts or when spawned
void AControlsTutorialActor::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AControlsTutorialActor::OnOverlapBegin);

}


void AControlsTutorialActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bOverlapped != true) {

		bOverlapped = true;

		SetActorHiddenInGame(false);
		GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &AControlsTutorialActor::OnLifeTimerEnded, LifeTime, false);

		if (NotificationSound != nullptr) {
			UGameplayStatics::SpawnSound2D(GetWorld(), NotificationSound, 1.0f, 1.0f, NotificationSoundStartTime);
		}

	}
}


void AControlsTutorialActor::OnLifeTimerEnded()
{

	Destroy();

}

