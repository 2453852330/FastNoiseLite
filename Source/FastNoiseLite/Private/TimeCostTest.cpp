// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeCostTest.h"

// Sets default values
ATimeCostTest::ATimeCostTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATimeCostTest::BeginPlay()
{
	Super::BeginPlay();
	double start_time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	
	NoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

	
	for (int32 i = 0; i < Count; ++i)
	{
		for (int32 j = 0; j < Count; ++j)
		{
			float color_value = NoiseLite.GetNoise<float>(i,j);
			DataList.Emplace(color_value);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[TimeCostTest->BeginPlay]get %d noise -> cost %f ms "),Count*Count,FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - start_time);
}

// Called every frame
void ATimeCostTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

