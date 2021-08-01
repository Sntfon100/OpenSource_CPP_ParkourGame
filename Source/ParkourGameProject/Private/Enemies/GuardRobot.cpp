// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/GuardRobot.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Characters/PlayerInterface.h"
#include "Engine/Public/TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundWave.h"
#include "General/BackgroundMusicActor.h"

// Sets default values
AGuardRobot::AGuardRobot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	RobotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotMesh"));
	RobotMesh->SetupAttachment(RootComponent);

	NewVisionField = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NewVisionField"));
	NewVisionField->SetupAttachment(RobotMesh);

	DestructEffect1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DestructEffect1"));
	DestructEffect1->SetupAttachment(RootComponent);
	DestructEffect1->SetAutoActivate(false);

	DestructEffect2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DestructEffect2"));
	DestructEffect2->SetupAttachment(RootComponent);
	DestructEffect2->SetAutoActivate(false);

	bSeesPlayer = false;

	bDisabled = false;

	TimeDilation = 0.1f;

	bSlowMotion = false;

	bGameOver = false;

	PatrolDelay = 1.0f;

	BustedDelay = 1.0f;

}



// Called when the game starts or when spawned
void AGuardRobot::BeginPlay()
{
	Super::BeginPlay();

	NewVisionField->OnComponentBeginOverlap.AddDynamic(this, &AGuardRobot::OnOverlapBegin);

	GetWorldTimerManager().SetTimer(LookUpTimerHandle, this, &AGuardRobot::LookUp, 0.01f, true, PatrolDelay);

}


void AGuardRobot::OnLookedAt_Implementation(AActor* Player)
{
	if (bSeesPlayer && bGameOver == false) {

		bDisabled = true;

		StopSlowmotion();

		UGameplayStatics::PlayWorldCameraShake(GetWorld(), ExplodeCamShake, Player->GetActorLocation(), 0.f, 500.0f, 1.0f);

		if (DestructSound1 != nullptr) {
			UGameplayStatics::SpawnSound2D(GetWorld(), DestructSound1);
		}

		if (DestructSound2 != nullptr) {
			UGameplayStatics::SpawnSound2D(GetWorld(), DestructSound2);
		}

		RobotMesh->SetHiddenInGame(true, true);

		SetActorEnableCollision(false);

		if (DestructEffect1 != nullptr) {

			DestructEffect1->Activate();

		}

		if (DestructEffect2 != nullptr) {

			DestructEffect2->Activate();

		}

		GetWorldTimerManager().SetTimer(DestroyDelayTimerHandle, this, &AGuardRobot::Destruct, 1.2f, false);

		//VisionField->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Casting to the background music actor to stop the music slowmotion effect (by changing the pitch back to 1)
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABackgroundMusicActor::StaticClass(), FoundActors);
		Cast<ABackgroundMusicActor>(FoundActors[0])->ChangePitch(1.0f);

	}

}

void AGuardRobot::OnStopLookedAt_Implementation()
{
	

}

void AGuardRobot::RotateToPlayer()
{

	FRotator LookAtRot;
	LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerRef->GetActorLocation());

	FRotator NewRobotRot;
	NewRobotRot = UKismetMathLibrary::InverseTransformRotation(RobotMesh->GetComponentTransform(), LookAtRot);
	RobotMesh->SetRelativeRotation(FRotator(0.f, (NewRobotRot.Yaw + 180), 0.f));

}


void AGuardRobot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {

		bSlowMotion = true;
		bSeesPlayer = true;
		PlayerRef = OtherActor;

		RotateToPlayer(); 
		StartSlowmotion();

		if(GetWorldTimerManager().IsTimerActive(LookUpTimerHandle))
		{

			GetWorldTimerManager().ClearTimer(LookUpTimerHandle);

		}else {

			GetWorldTimerManager().ClearTimer(LookDownTimerHandle);

		}

		//GetWorldTimerManager().SetTimer(LookAtPlayerTimerHandle, this, &AGuardRobot::RotateToPlayer, (0.001f * TimeDilation), true);


	}
}


void AGuardRobot::StartSlowmotion()
{

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
	GetWorldTimerManager().SetTimer(SlowMotionTimerHandle, this, &AGuardRobot::EndGame, (BustedDelay * TimeDilation), false);

	// Casting to the background music actor to add slowmotion effect for the music (by changing the pitch to be < 0)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABackgroundMusicActor::StaticClass(), FoundActors);
	Cast<ABackgroundMusicActor>(FoundActors[0])->ChangePitch(0.8f);

}

void AGuardRobot::EndGame()
{

	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);

	bGameOver = true;

	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(PlayerRef);

	if (PlayerInterface) {
		PlayerInterface->Execute_GameOver(PlayerRef, TimeDilation);
	}


}

void AGuardRobot::StopSlowmotion()
{

	if (bSlowMotion && bGameOver != true) {

		bSlowMotion = false;

		GetWorldTimerManager().PauseTimer(SlowMotionTimerHandle);
		GetWorldTimerManager().ClearTimer(SlowMotionTimerHandle);

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	}


}

void AGuardRobot::LookUp()
{
	FRotator NewRot = FRotator(0.f, 0.f, 1.0f);

	RobotMesh->AddRelativeRotation(NewRot);

	if (RobotMesh->GetRelativeRotation().Roll >= 45) {
		
		GetWorldTimerManager().ClearTimer(LookUpTimerHandle);
		GetWorldTimerManager().SetTimer(LookDownTimerHandle, this, &AGuardRobot::LookDown, 0.01f, true, PatrolDelay);

	}
}

void AGuardRobot::LookDown()
{
	FRotator NewRot = FRotator(0.f, 0.f, -1.0f);

	RobotMesh->AddRelativeRotation(NewRot);

	if (RobotMesh->GetRelativeRotation().Roll <= -45) {

		GetWorldTimerManager().ClearTimer(LookDownTimerHandle);
		GetWorldTimerManager().SetTimer(LookUpTimerHandle, this, &AGuardRobot::LookUp, 0.01f, true, PatrolDelay);

	}
}

void AGuardRobot::Destruct()
{

	Destroy();

}
