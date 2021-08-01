// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/LevelFinish.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Public/TimerManager.h"
#include "Characters/ParkourCharacter.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ALevelFinish::ALevelFinish()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;

	LevelChangeDelay = 2.5f;
	FadeDuration = 2.0f;

}

// Called when the game starts or when spawned
void ALevelFinish::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelFinish::OnOverlapBegin);
	
}

void ALevelFinish::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// Creating a Reference to the Player Controller
	APlayerController* const PC = UGameplayStatics::GetPlayerController(this, 0);

	// Disabling Player Input
	Cast<AParkourCharacter>(PC->GetPawn())->DisableInput(PC);

	// Check if Last Level
	if (NextLevel == "End") {

		if (IsValid(EndScreenWidgetClass)) {

			EndScreenWidget = CreateWidget(GetWorld(), EndScreenWidgetClass);

			if (EndScreenWidget != nullptr) {

				EndScreenWidget->AddToViewport(1);
			}

		}

		PC->SetInputMode(FInputModeUIOnly());
		PC->SetShowMouseCursor(true);

	}
	else {

		// Starting a camera fade
		PC->PlayerCameraManager->StartCameraFade(0.f, 1.0f, FadeDuration, FLinearColor::Black, true, true);

		// Starting a timer to delay the level change
		GetWorldTimerManager().SetTimer(LevelChangeTimerHandle, this, &ALevelFinish::ChangeLevel, LevelChangeDelay, false);

	}


	


}

void ALevelFinish::ChangeLevel()
{

	UGameplayStatics::OpenLevel(GetWorld(), NextLevel);

}


