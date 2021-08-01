// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Characters/PlayerInterface.h"
#include "ParkourCharacter.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Forward Declerations
class USpringArmComponent;
class UCameraComponent;
class UCurveFloat;
class USoundBase;
class UAudioComponent;
class UMatineeCameraShake;
class UUserWidget;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class PARKOURGAMEPROJECT_API AParkourCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	// Sets default values for this character's properties

	AParkourCharacter();

	// Called every frame

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Components Setup

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	// Movement Functions

	UFUNCTION()
	void DoubleJump();

	UFUNCTION()
	void Dash();
	UFUNCTION()
	void OnDashEnded();
	UFUNCTION()
	void StopDash();

	UFUNCTION(BlueprintCallable, Category = "Movement")
		FVector GetDesiredMovement();

	// Movement Variables

	UPROPERTY(EditAnyWhere, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnyWhere, Category = "Movement")
	float SprintSpeed;

	float MoveForwardScale;
	float MoveRightScale;

	UPROPERTY(EditAnyWhere, Category = "Movement")
	float JumpHeight;
	UPROPERTY(EditAnyWhere, Category = "Movement")
	float JumpCounter;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Movement")
	float WalkDashDistance;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Movement")
	float SprintDashDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DashDistance;
	UPROPERTY(EditAnyWhere, Category = "Movement")
	float DashCoolDown;
	UPROPERTY(EditAnyWhere, Category = "Movement")
	float DashStopTime;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool CanDash;

	// Interface functions

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void GameOver(float TimeDilation);
	virtual void GameOver_Implementation(float TimeDilation); 

	UFUNCTION()
	void CallGameOver(AActor* DestroyedActor);

	//GameOver Variables

	UPROPERTY(EditAnyWhere, Category = "GameOver");
	float FadeDuration;

	FName CurrentLevelName;

	// GameOver Functions

	void RestartLevel();

	// Sound Variables

	UPROPERTY(EditAnyWhere, Category = "Sound")
	USoundBase* FootstepSound;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	USoundBase* LandSound;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	USoundBase* DashSound;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	float LandSoundStartTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:

	// Called when the game starts or when spawned

	virtual void BeginPlay() override;

	// Movement Functions

	void MoveForward(float Value);
	void MoveRight(float Value);

	virtual void Landed(const FHitResult& Hit) override;

	// Trace Functions

	UFUNCTION()
	void Trace();

	UFUNCTION()
	void FootStepTrace(bool Landed);

	// Trace Variables

	UPROPERTY(EditAnyWhere, Category = "Trace")
	float TraceRadius;

	// Timers

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	FTimerHandle RestartLevelHandle;

	// Sound Variables

	UAudioComponent* SpawnedFootStepSound;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	float FootStepsSoundVol;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	float LandSoundVol;
	
	// Camera Shake

	UPROPERTY(EditAnyWhere, Category = "CameraShake")
	TSubclassOf<UMatineeCameraShake> WalkCamShake;

	UPROPERTY(EditAnyWhere, Category = "CameraShake")
	TSubclassOf<UMatineeCameraShake> LandCamShake;

	// Gameplay Functions

	void PauseGame();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY()
	UUserWidget* PauseWidget;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:

	// Timeline Variables

	UPROPERTY(EditAnyWhere, Category = "Timeline")
	float TimeLineStartFOV;
	UPROPERTY(EditAnyWhere, Category = "Timeline")
	float TimeLineEndFOV;

	UPROPERTY()
	FTimeline FOVTimeLine;

	UPROPERTY(EditAnyWhere, Category = "Timeline")
	UCurveFloat* FOVCurveFloat;

	// Timeline Functions

	UFUNCTION()
	void FOVTimeLineProgress(float value);

	// Sound Variables

	UPROPERTY(EditAnyWhere, Category = "Sound")
	float StepSize;

	UPROPERTY(EditAnyWhere, Category = "Sound")
	float FootStepsFadeOutDuration;

	// General Variables

	APlayerController* MyPC;

};
