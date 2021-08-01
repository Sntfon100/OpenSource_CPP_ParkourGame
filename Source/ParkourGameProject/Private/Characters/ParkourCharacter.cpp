// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ParkourCharacter.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFrameWork/Controller.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Engine/Public/TimerManager.h"
#include "Engine/Classes/Engine/World.h"
#include "Components/TimelineComponent.h"
#include "Enemies/GuardRobotInterface.h"
#include "DrawDebugHelpers.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Sets default values
AParkourCharacter::AParkourCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SprintSpeed = 800.0f;
	WalkSpeed = 400.0f;

	JumpHeight = 300.0f;

	WalkDashDistance = 500.0f;
	SprintDashDistance = 1000.0f;

	DashCoolDown = 0.5f;
	DashStopTime = 0.1f;
	CanDash = true;

	TimeLineEndFOV = 110.0f;

	FadeDuration = 1.0f;

	StepSize = 125.0f;

	LandSoundStartTime = 0.08f;

	FootStepsFadeOutDuration = 0.5f;

	TraceRadius = 10.0f;

	FootStepsSoundVol = 0.5f;

	LandSoundVol = 0.5f;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called when the game starts or when spawned
void AParkourCharacter::BeginPlay()
{
	Super::BeginPlay();

	//PrimaryActorTick.bCanEverTick = true;

	DashDistance = SprintDashDistance;

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	TimeLineStartFOV = Camera->FieldOfView;

	//OnDestroyed.AddDynamic(this, &AParkourCharacter::CallGameOver);

	if (FOVCurveFloat) {

		FOnTimelineFloat TimeLineProgress;
		TimeLineProgress.BindUFunction(this, FName("FOVTimeLineProgress"));
		FOVTimeLine.AddInterpFloat(FOVCurveFloat, TimeLineProgress);
		FOVTimeLine.SetLooping(false);

	}

	MyPC = UGameplayStatics::GetPlayerController(this, 0); // Creating a reference to the Player Controller


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called every frame
void AParkourCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FOVTimeLine.TickTimeline(DeltaTime);

	Trace();

	//GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Emerald, FString::Printf(TEXT("Time Dilation is: %f"), UGameplayStatics::TimeDilation));

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called to bind functionality to input
void AParkourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AParkourCharacter::DoubleJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AParkourCharacter::Dash);

	// Move Forward, Backward, Right, Left
	PlayerInputComponent->BindAxis("MoveForward", this, &AParkourCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AParkourCharacter::MoveRight);

	// Look Right, Left, Up, Down
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Pause
	PlayerInputComponent->BindAction("PauseGame", IE_Pressed,this, &AParkourCharacter::PauseGame).bExecuteWhenPaused = true;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AParkourCharacter::MoveForward(float Value)
{
	MoveForwardScale = Value;

	if ((Controller != nullptr) && (Value != 0.0f)) {

		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator ControllerYawRotation(0, ControllerRotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(ControllerYawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		FootStepTrace(false);

		if (GetMovementComponent()->IsMovingOnGround()) {

			float WalkCamShakeScale = GetVelocity().Size() / SprintSpeed;
			MyPC->PlayerCameraManager->PlayCameraShake(WalkCamShake, WalkCamShakeScale);
			
		}

	}
}

void AParkourCharacter::MoveRight(float Value)
{
	MoveRightScale = Value;

	if ((Controller != NULL) && (Value != 0)) {


		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator ControllerYawRotation(0, ControllerRotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(ControllerYawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		FootStepTrace(false);

	}

}

void AParkourCharacter::DoubleJump()
{

	if (SpawnedFootStepSound && JumpCounter == 0 && SpawnedFootStepSound->IsActive()) 
	{
		//SpawnedFootStepSound->Deactivate();
		SpawnedFootStepSound->FadeOut(FootStepsFadeOutDuration, 0.0f);
	}

	if (JumpCounter <= 1) 
	{
		LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
		JumpCounter++;
	}

}

void AParkourCharacter::Landed(const FHitResult& Hit)
{

	Super::Landed(Hit);

	JumpCounter = 0;

	FootStepTrace(true);

	float LandCamShakeScale = floor((((GetVelocity().Z * -1.0f) / 312) + 0.8));

	if (Controller != nullptr) {
		MyPC->PlayerCameraManager->PlayCameraShake(LandCamShake, LandCamShakeScale);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("Scale = %f"), LandCamShakeScale));


}

void AParkourCharacter::Dash()
{
	if (CanDash == true) {

		CanDash = false;

		GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
		GetCharacterMovement()->Velocity = FVector(0, 0, 0);

		if (GetDesiredMovement().Size() != 0) {
			LaunchCharacter(GetDesiredMovement() * DashDistance, true, true);
		}
		else { LaunchCharacter(GetActorForwardVector() * DashDistance, true, true); }

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AParkourCharacter::StopDash, DashStopTime, false);

		FOVTimeLine.PlayFromStart();

		if (DashSound != nullptr) {

			float TimeDilation =  UGameplayStatics::GetGlobalTimeDilation(this);
			UGameplayStatics::SpawnSound2D(GetWorld(), DashSound, 1.0f, TimeDilation);

		}
	}
}

void AParkourCharacter::StopDash()
{

	GetCharacterMovement()->BrakingFrictionFactor = 2.0f;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AParkourCharacter::OnDashEnded, DashCoolDown, false);

}

void AParkourCharacter::OnDashEnded()
{

	CanDash = true;

}

FVector AParkourCharacter::GetDesiredMovement()
{

	FVector DesiredMovementVector;
	DesiredMovementVector = GetActorForwardVector() * MoveForwardScale + GetActorRightVector() * MoveRightScale;
	DesiredMovementVector.Normalize(0.001f);
	return DesiredMovementVector;

}

void AParkourCharacter::PauseGame()
{

	if (UGameplayStatics::IsGamePaused(this) != true) {

		if (IsValid(PauseWidgetClass)) {

			PauseWidget = CreateWidget(GetWorld(), PauseWidgetClass);

			if (PauseWidget != nullptr) {

				PauseWidget->AddToViewport(1);

			}
		}

		UGameplayStatics::SetGamePaused(this, true);

	}
	else {

		UGameplayStatics::SetGamePaused(this, false);
		
		if (PauseWidget != nullptr) {

			PauseWidget->RemoveFromParent();
			PauseWidget = nullptr;

			MyPC->SetInputMode(FInputModeGameOnly());
			MyPC->SetShowMouseCursor(false);

		}

	}

}

void AParkourCharacter::FOVTimeLineProgress(float value)
{

	float NewFOV = FMath::Lerp(TimeLineStartFOV, TimeLineEndFOV, value);

	Camera->FieldOfView = NewFOV;

}

void AParkourCharacter::Trace()
{
	if (GetController())
	{
		static FVector CamLoc;
		static FRotator CamRot;
		GetController()->GetPlayerViewPoint(CamLoc, CamRot);

		static FVector LT_Start;
		LT_Start = CamLoc;
		static FVector LT_End;
		LT_End = LT_Start + (CamRot.Vector() * 10000);

		static FHitResult LT_HitResult;
		static FCollisionQueryParams LT_CollisionParams;

		static bool bHit;

		static AActor* HitActor;
		static AActor* FocusedActor;

		static bool bHasGuardRobotInterface = false;

		static const TArray<AActor*> ActorsToIgnore;

		//bHit = GetWorld()->LineTraceSingleByChannel(LT_HitResult, LT_Start, LT_End, ECC_Visibility, LT_CollisionParams);
		
		bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), LT_Start, LT_End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, LT_HitResult, true);

		//DrawDebugLine(GetWorld(), LT_Start, LT_End, FColor::Red, false, 2.0f);

		if (bHit == true) 
		{

			//DrawDebugBox(GetWorld(), LT_HitResult.ImpactPoint, FVector(4, 4, 4), FColor::Blue, false, 2.0f);

			HitActor = LT_HitResult.GetActor();

			if (HitActor) 
			{

				bHasGuardRobotInterface = HitActor->GetClass()->ImplementsInterface(UGuardRobotInterface::StaticClass());

				if (HitActor != FocusedActor) {

					if (FocusedActor != nullptr && bHasGuardRobotInterface) 
					{

						IGuardRobotInterface* GuardRobotInterface = Cast<IGuardRobotInterface>(FocusedActor);

						if (GuardRobotInterface != nullptr) {

							GuardRobotInterface->Execute_OnStopLookedAt(FocusedActor);

						}

					}
					
					if (bHasGuardRobotInterface) 
					{
						IGuardRobotInterface* GuardRobotInterface = Cast<IGuardRobotInterface>(HitActor);

						if (GuardRobotInterface) {

							GuardRobotInterface->Execute_OnLookedAt(HitActor, this);

						}
					}

					FocusedActor = HitActor;

				}

			}

		}
		else if (FocusedActor != nullptr)
		{
			
			if (bHasGuardRobotInterface) 
			{

				IGuardRobotInterface* GuardRobotInterface = Cast<IGuardRobotInterface>(FocusedActor);

				if (GuardRobotInterface != nullptr) {

					//GuardRobotInterface->Execute_OnStopLookedAt(FocusedActor);

				}

			}

			FocusedActor = NULL;

			bHasGuardRobotInterface = false;

		}

	}

}

void AParkourCharacter::GameOver_Implementation(float TimeDilation)
{
	if (Controller != nullptr) {

		MyPC->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), (FadeDuration * 0.1), true, true); // Starting a Camera fade
		
		CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(GetWorld())); // Converting String to Name

		GetWorldTimerManager().SetTimer(RestartLevelHandle, this, &AParkourCharacter::RestartLevel, ((FadeDuration * TimeDilation) + (1 * TimeDilation)), false);

	}
}

void AParkourCharacter::CallGameOver(AActor* DestroyedActor)
{

	GameOver(1.0f);

}

void AParkourCharacter::RestartLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName);
}

void AParkourCharacter::FootStepTrace(bool Landed)
{
	static FVector CurrentFootLocation;

	if (Landed == false)
	{

		if (FootstepSound && GetMovementComponent()->IsMovingOnGround()) 
		{

			static float FootDistance;

			FootDistance = FVector(GetActorLocation() - CurrentFootLocation).Size();
			//GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, FString::Printf(TEXT("Foot Distance = %f"), FootDistance));

			if (FootDistance > StepSize && GetMovementComponent()->IsMovingOnGround() && GetVelocity().Size() > 800.f) 
			{

				static FVector LT_Start;
				static FVector LT_End;

				LT_Start = GetActorLocation();
				LT_End = LT_Start - FVector(0, 0, 150);

				static FHitResult LT_HitResult;
				static FCollisionQueryParams LT_CollisionParams;

				static bool bHit;

				bHit = GetWorld()->LineTraceSingleByChannel(LT_HitResult, LT_Start, LT_End, ECC_Visibility, LT_CollisionParams);
				//DrawDebugLine(GetWorld(), LT_Start, LT_End, FColor::Red, false, 2.0f);

				if (bHit == true) 
				{
					SpawnedFootStepSound = UGameplayStatics::SpawnSound2D(GetWorld(), FootstepSound, FootStepsSoundVol);

					CurrentFootLocation = LT_HitResult.Location;
				}
			}
		}
		
	} else {
				if (LandSound) 
				{
					CurrentFootLocation = GetActorLocation();
					UGameplayStatics::SpawnSound2D(GetWorld(), LandSound, LandSoundVol, 1.0f, LandSoundStartTime);
				}
			}
}
