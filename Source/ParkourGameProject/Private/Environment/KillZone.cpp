// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/KillZone.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerInterface.h"

// Sets default values
AKillZone::AKillZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AKillZone::OnOverlapBegin);

	TimeDilation = 0.1f;
}

// Called when the game starts or when spawned
void AKillZone::BeginPlay()
{
	Super::BeginPlay();
}


void AKillZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);

		IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(OtherActor);

		if (PlayerInterface) {

			PlayerInterface->Execute_GameOver(OtherActor, TimeDilation);

		}

	}

}

