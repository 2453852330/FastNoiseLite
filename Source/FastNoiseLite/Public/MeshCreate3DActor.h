// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseBPType.h"

#include "GameFramework/Actor.h"
#include "MeshCreate3DActor.generated.h"

class UFastNoiseObject;
class UInstancedStaticMeshComponent;


UCLASS()
class FASTNOISELITE_API AMeshCreate3DActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshCreate3DActor();

	virtual void BeginPlay() override;


	UPROPERTY()
	UFastNoiseObject* FastNoiseObject = nullptr;

	UPROPERTY(EditAnywhere,Category="MeshCreate3DActor")
	FIntVector Size = FIntVector(10);
	
	UPROPERTY(EditDefaultsOnly,Category="MeshCreate3DActor")
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere,Category="MeshCreate3DActor")
	float Span = 100.f;

	UPROPERTY(EditAnywhere,Category="MeshCreate3DActor")
	float HeightScale = 1.f;


	FTransform CF_GetTransformFromXYZ(int32 X,int32 Y,int32 Z,float Value);
	UPROPERTY(EditAnywhere,Category="MeshCreate3DActor")
	FNoiseCreateParams NoiseCreateParams;
	UPROPERTY(EditAnywhere,Category="MeshCreate3DActor")
	float CheckValue_3D = 0.f;
	
	UPROPERTY(EditAnywhere,Category="MeshCreate3DActor")
	bool b2D = true;
};
