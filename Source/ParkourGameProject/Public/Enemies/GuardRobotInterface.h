// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GuardRobotInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGuardRobotInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PARKOURGAMEPROJECT_API IGuardRobotInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GuardRobot")
		void OnLookedAt(AActor* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GuardRobot")
		void OnStopLookedAt();

};
