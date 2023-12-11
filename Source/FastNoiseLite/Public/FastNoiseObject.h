// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseBPType.h"

#include "UObject/Object.h"
#include "FastNoiseLite_Library.h"
#include "FastNoiseObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class FASTNOISELITE_API UFastNoiseObject : public UObject
{
	GENERATED_BODY()

public:

	FastNoiseLite NoiseLite;

	UFUNCTION(BlueprintCallable,Category="FastNoiseObject")
	void BP_SetNoiseCreateParam(FNoiseCreateParams CreateParams);
	
	UFUNCTION(BlueprintCallable,Category="FastNoiseObject")
	UTextureRenderTarget2D* BP_GetRenderTarget2D();

	UFUNCTION(BlueprintCallable,Category="FastNoiseObject")
	void BP_FillDataToRenderTarget(UTextureRenderTarget2D* RenderTarget);

	UFUNCTION(BlueprintCallable,Category="FastNoiseObject")
	float BP_GetNoiseValue2D(int32 X,int32 Y);

	UFUNCTION(BlueprintCallable,Category="FastNoiseObject")
	void BP_FillDataToRenderTargetThread(UTextureRenderTarget2D* RenderTarget);

	UPROPERTY(BlueprintReadWrite,Category="FastNoiseObject")
	EDrawThreadMethod DrawThreadMethod = EDrawThreadMethod::EMethod1;
	
	UPROPERTY(BlueprintReadWrite,Category="FastNoiseObject")
	float Thickness = 0.f;	

	UFUNCTION(BlueprintCallable,Category="FastNoiseObject")
	void BP_SetDrawThreadParam(EDrawThreadMethod InDrawThreadMethod,float InThickness);
	
	FNoiseCreateParams CacheCreateParams;
private:
	FastNoiseLite::NoiseType ConvertNoise(EFastNoiseBPType NoiseBP) const;
	FastNoiseLite::FractalType ConvertFractal(EFractalBPType FractalBP) const;
	FastNoiseLite::RotationType3D ConvertRotationType3D(ERotationType3DBP RotationType3D) const;
	FastNoiseLite::CellularDistanceFunction ConvertDistanceFunction(ECellularDistanceFunctionBP DistanceFunctionBP) const;
	FastNoiseLite::CellularReturnType ConvertReturnType( ECellularReturnTypeBP CellularReturnBP) const;
	FastNoiseLite::DomainWarpType ConvertDomainWarpType( EDomainWarpBPType DomainWarpBP) const;
};
