// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastNoiseLite_Library.h"
#include "TimeCostTest.generated.h"

UCLASS()
class FASTNOISELITE_API ATimeCostTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeCostTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	FastNoiseLite NoiseLite;

	UPROPERTY(EditAnywhere)
	int32 Count = 100;

	TArray<float> DataList;
};
