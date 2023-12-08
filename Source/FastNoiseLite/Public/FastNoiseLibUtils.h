// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseObject.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FastNoiseLibUtils.generated.h"

/**
 * 
 */
UCLASS()
class FASTNOISELITE_API UFastNoiseLibUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,Category="FastNoiseLib")
	static UFastNoiseObject* Lib_CreateFastNoiseObject(UObject* Owner);
};
