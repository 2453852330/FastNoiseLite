#pragma once

#include "FastNoiseBPType.generated.h"


UENUM()
enum class EFastNoiseBPType : uint8
{
	NoiseType_OpenSimplex2,
	NoiseType_OpenSimplex2S,
	NoiseType_Cellular,
	NoiseType_Perlin,
	NoiseType_ValueCubic,
	NoiseType_Value
};

UENUM()
enum class EFractalBPType : uint8
{
	FractalType_None,
	FractalType_FBm,
	FractalType_Ridged,
	FractalType_PingPong,
	FractalType_DomainWarpProgressive,
	FractalType_DomainWarpIndependent
};


UENUM()
enum class ERotationType3DBP : uint8
{
	RotationType3D_None,
	RotationType3D_ImproveXYPlanes,
	RotationType3D_ImproveXZPlanes
};


UENUM()
enum class ECellularDistanceFunctionBP : uint8
{
	CellularDistanceFunction_Euclidean,
	CellularDistanceFunction_EuclideanSq,
	CellularDistanceFunction_Manhattan,
	CellularDistanceFunction_Hybrid
};


UENUM()
enum class ECellularReturnTypeBP : uint8
{
	CellularReturnType_CellValue,
	CellularReturnType_Distance,
	CellularReturnType_Distance2,
	CellularReturnType_Distance2Add,
	CellularReturnType_Distance2Sub,
	CellularReturnType_Distance2Mul,
	CellularReturnType_Distance2Div
};

UENUM()
enum class EDomainWarpBPType : uint8
{
	DomainWarpType_OpenSimplex2,
	DomainWarpType_OpenSimplex2Reduced,
	DomainWarpType_BasicGrid
};

USTRUCT(BlueprintType)
struct FNoiseCreateParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	EFastNoiseBPType NoiseType = EFastNoiseBPType::NoiseType_OpenSimplex2;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	int32 Seed = 1337;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	ERotationType3DBP RotationType3D = ERotationType3DBP::RotationType3D_None;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	float Frequency = 0.01f;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	EFractalBPType FractalType = EFractalBPType::FractalType_None;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	int32 FractalOctaves = 3;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalLacunarity = 2.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalGain = 0.5f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalWeightedStrength = 0.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalPingPongStrength = 2.f;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Cellular")
	ECellularDistanceFunctionBP CellularDistanceFunction = ECellularDistanceFunctionBP::CellularDistanceFunction_Euclidean;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Cellular")
	ECellularReturnTypeBP CellularReturnType = ECellularReturnTypeBP::CellularReturnType_Distance;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Cellular")
	float CellularJitter = 1.f;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|DomainWarp")
	EDomainWarpBPType DomainWarpType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|DomainWarp")
	float DomainWarpAmp =  1.f;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Size")
	FIntPoint Size2D = FIntPoint(128);
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Size")
	FIntVector Size3D = FIntVector(32);	
};
