// Fill out your copyright notice in the Description page of Project Settings.


#include "FastNoiseObject.h"

#include "CanvasTypes.h"
#include "DrawRenderTargetThread.h"
#include "FastNoiseBPType.h"

#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"

void UFastNoiseObject::BP_SetNoiseCreateParam(FNoiseCreateParams CreateParams)
{
	CacheCreateParams = CreateParams;
	// general
	NoiseLite.SetNoiseType(ConvertNoise(CreateParams.NoiseType));
	NoiseLite.SetFrequency(CreateParams.Frequency);
	NoiseLite.SetSeed(CreateParams.Seed);
	NoiseLite.SetRotationType3D(ConvertRotationType3D(CreateParams.RotationType3D));
	// fractal
	NoiseLite.SetFractalType(ConvertFractal(CreateParams.FractalType));
	NoiseLite.SetFractalOctaves(CreateParams.FractalOctaves);
	NoiseLite.SetFractalLacunarity(CreateParams.FractalLacunarity);
	NoiseLite.SetFractalGain(CreateParams.FractalGain);
	NoiseLite.SetFractalWeightedStrength(CreateParams.FractalWeightedStrength);
	NoiseLite.SetFractalPingPongStrength(CreateParams.FractalPingPongStrength);
	// cellular
	NoiseLite.SetCellularDistanceFunction(ConvertDistanceFunction(CreateParams.CellularDistanceFunction));
	NoiseLite.SetCellularReturnType(ConvertReturnType(CreateParams.CellularReturnType));
	NoiseLite.SetCellularJitter(CreateParams.CellularJitter);
	// domain warp : rotation type + seed + frequency 
	NoiseLite.SetDomainWarpType(ConvertDomainWarpType(CreateParams.DomainWarpType));
	NoiseLite.SetDomainWarpAmp(CreateParams.DomainWarpAmp);
	// Domain Warp Fractal : Type + Octaves + Lacunarity + Gain;
}

UTextureRenderTarget2D* UFastNoiseObject::BP_GetRenderTarget2D()
{
	// create
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->bCanCreateUAV = true;
	RenderTarget->InitAutoFormat(CacheCreateParams.Size2D.X, CacheCreateParams.Size2D.Y);
	RenderTarget->UpdateResourceImmediate(true);
	// draw
	UWorld* World = GetWorld();
	World->FlushDeferredParameterCollectionInstanceUpdates();


	UCanvas* Canvas = World->GetCanvasForRenderingToTarget();

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FCanvas* NewCanvas = new FCanvas(
		RenderTargetResource,
		nullptr,
		World,
		World->GetFeatureLevel(),
		// Draw immediately so that interleaved SetVectorParameter (etc) function calls work as expected
		FCanvas::CDM_ImmediateDrawing);

	Canvas->Init(CacheCreateParams.Size2D.X, CacheCreateParams.Size2D.Y, nullptr, NewCanvas);

	for (int32 i = 0; i < CacheCreateParams.Size2D.X; ++i)
	{
		for (int32 j = 0; j < CacheCreateParams.Size2D.Y; ++j)
		{
			FCanvasBoxItem BoxItem(FVector2d(i, j), FVector2d(1.f));
			float noise_value = NoiseLite.GetNoise<float>(i, j);
			BoxItem.SetColor(FLinearColor(noise_value, noise_value, noise_value, 1.f));
			BoxItem.LineThickness = 1.f;
			Canvas->DrawItem(BoxItem);
		}
	}


	ENQUEUE_RENDER_COMMAND(FlushDeferredResourceUpdateCommand)(
		[RenderTargetResource](FRHICommandListImmediate& RHICmdList)
		{
			RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);
		});


	return RenderTarget;
}

void UFastNoiseObject::BP_FillDataToRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	if (!RenderTarget)
	{
		return;
	}

	int32 X = RenderTarget->SizeX;
	int32 Y = RenderTarget->SizeY;

	double start_time = FPlatformTime::Seconds();
	
	UWorld* World = GetWorld();
	World->FlushDeferredParameterCollectionInstanceUpdates();


	UCanvas* Canvas = World->GetCanvasForRenderingToTarget();

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FCanvas* NewCanvas = new FCanvas(
		RenderTargetResource,
		nullptr,
		World,
		World->GetFeatureLevel(),
		// Draw immediately so that interleaved SetVectorParameter (etc) function calls work as expected
		FCanvas::CDM_ImmediateDrawing);

	Canvas->Init(X, Y, nullptr, NewCanvas);

	float noise_value = 0.f;
	FCanvasBoxItem BoxItem(FVector2d::ZeroVector,FVector2d(1.f));
	BoxItem.LineThickness = 1.f;
	
	for (int32 i = 0; i < X; ++i)
	{
		for (int32 j = 0; j < Y; ++j)
		{
			BoxItem.Position = FVector2d(i, j);
			noise_value = NoiseLite.GetNoise<float>(i, j);
			BoxItem.SetColor(FLinearColor(noise_value, noise_value, noise_value, 1.f));
			Canvas->DrawItem(BoxItem);
		}
	}


	ENQUEUE_RENDER_COMMAND(FlushDeferredResourceUpdateCommand)(
		[RenderTargetResource](FRHICommandListImmediate& RHICmdList)
		{
			RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);
		});

	UE_LOG(LogTemp, Warning, TEXT("[FastNoiseObject->BP_FillDataToRenderTarget]cost %f time"),FPlatformTime::Seconds() - start_time);
}

float UFastNoiseObject::BP_GetNoiseValue2D(int32 X, int32 Y)
{
	return NoiseLite.GetNoise<float>(X,Y);
}

void UFastNoiseObject::BP_FillDataToRenderTargetThread(UTextureRenderTarget2D* RenderTarget)
{
	new FDrawRenderTargetThread(this,RenderTarget,GetWorld()->GetCanvasForRenderingToTarget(),DrawThreadMethod,Thickness);
}

void UFastNoiseObject::BP_SetDrawThreadParam(EDrawThreadMethod InDrawThreadMethod, float InThickness)
{
	DrawThreadMethod = InDrawThreadMethod;
	Thickness = InThickness;
}

FastNoiseLite::NoiseType UFastNoiseObject::ConvertNoise(EFastNoiseBPType NoiseBP) const
{
	switch (NoiseBP)
	{
	case EFastNoiseBPType::NoiseType_Cellular:
		{
			return FastNoiseLite::NoiseType_Cellular;
		}
	case EFastNoiseBPType::NoiseType_Perlin:
		{
			return FastNoiseLite::NoiseType_Perlin;
		}
	case EFastNoiseBPType::NoiseType_Value:
		{
			return FastNoiseLite::NoiseType_Value;
		}
	case EFastNoiseBPType::NoiseType_OpenSimplex2:
		{
			return FastNoiseLite::NoiseType_OpenSimplex2;
		}
	case EFastNoiseBPType::NoiseType_ValueCubic:
		{
			return FastNoiseLite::NoiseType_ValueCubic;
		}
	case EFastNoiseBPType::NoiseType_OpenSimplex2S:
		{
			return FastNoiseLite::NoiseType_OpenSimplex2S;
		}
	}

	return FastNoiseLite::NoiseType_Perlin;
}

FastNoiseLite::FractalType UFastNoiseObject::ConvertFractal(EFractalBPType FractalBP) const
{
	switch (FractalBP)
	{
	case EFractalBPType::FractalType_None:
		{
			return FastNoiseLite::FractalType_None;
		}
	case EFractalBPType::FractalType_Ridged:
		{
			return FastNoiseLite::FractalType_Ridged;
		}
	case EFractalBPType::FractalType_FBm:
		{
			return FastNoiseLite::FractalType_FBm;
		}
	case EFractalBPType::FractalType_PingPong:
		{
			return FastNoiseLite::FractalType_PingPong;
		}
	case EFractalBPType::FractalType_DomainWarpIndependent:
		{
			return FastNoiseLite::FractalType_DomainWarpIndependent;
		}
	case EFractalBPType::FractalType_DomainWarpProgressive:
		{
			return FastNoiseLite::FractalType_DomainWarpProgressive;
		}
	}
	return FastNoiseLite::FractalType_None;
}

FastNoiseLite::RotationType3D UFastNoiseObject::ConvertRotationType3D(ERotationType3DBP RotationType3D) const
{
	switch (RotationType3D)
	{
	case ERotationType3DBP::RotationType3D_None:
		{
			return FastNoiseLite::RotationType3D_None;
		}
	case ERotationType3DBP::RotationType3D_ImproveXYPlanes:
		{
			return FastNoiseLite::RotationType3D_ImproveXYPlanes;
		}
	case ERotationType3DBP::RotationType3D_ImproveXZPlanes:
		{
			return FastNoiseLite::RotationType3D_ImproveXZPlanes;
		}
	}
	return FastNoiseLite::RotationType3D_None;
}

FastNoiseLite::CellularDistanceFunction UFastNoiseObject::ConvertDistanceFunction(ECellularDistanceFunctionBP DistanceFunctionBP) const
{
	switch (DistanceFunctionBP)
	{
	case ECellularDistanceFunctionBP::CellularDistanceFunction_Euclidean:
		{
			return FastNoiseLite::CellularDistanceFunction_Euclidean;
		}
	case ECellularDistanceFunctionBP::CellularDistanceFunction_Hybrid:
		{
			return FastNoiseLite::CellularDistanceFunction_Hybrid;
		}
	case ECellularDistanceFunctionBP::CellularDistanceFunction_Manhattan:
		{
			return FastNoiseLite::CellularDistanceFunction_Manhattan;
		}
	case ECellularDistanceFunctionBP::CellularDistanceFunction_EuclideanSq:
		{
			return FastNoiseLite::CellularDistanceFunction_EuclideanSq;
		}
	}
	return FastNoiseLite::CellularDistanceFunction_Euclidean;
}

FastNoiseLite::CellularReturnType UFastNoiseObject::ConvertReturnType(ECellularReturnTypeBP CellularReturnBP) const
{
	switch (CellularReturnBP)
	{
	case ECellularReturnTypeBP::CellularReturnType_Distance:
		{
			return FastNoiseLite::CellularReturnType_Distance;
		}
	case ECellularReturnTypeBP::CellularReturnType_Distance2:
		{
			return FastNoiseLite::CellularReturnType_Distance2;
		}
	case ECellularReturnTypeBP::CellularReturnType_CellValue:
		{
			return FastNoiseLite::CellularReturnType_CellValue;
		}
	case ECellularReturnTypeBP::CellularReturnType_Distance2Add:
		{
			return FastNoiseLite::CellularReturnType_Distance2Add;
		}
	case ECellularReturnTypeBP::CellularReturnType_Distance2Div:
		{
			return FastNoiseLite::CellularReturnType_Distance2Div;
		}
	case ECellularReturnTypeBP::CellularReturnType_Distance2Mul:
		{
			return FastNoiseLite::CellularReturnType_Distance2Mul;
		}
	case ECellularReturnTypeBP::CellularReturnType_Distance2Sub:
		{
			return FastNoiseLite::CellularReturnType_Distance2Sub;
		}
	}

	return FastNoiseLite::CellularReturnType_Distance;
}

FastNoiseLite::DomainWarpType UFastNoiseObject::ConvertDomainWarpType(EDomainWarpBPType DomainWarpBP) const
{
	switch (DomainWarpBP)
	{
	case EDomainWarpBPType::DomainWarpType_BasicGrid:
		{
			return FastNoiseLite::DomainWarpType_BasicGrid;
		}
	case EDomainWarpBPType::DomainWarpType_OpenSimplex2:
		{
			return FastNoiseLite::DomainWarpType_OpenSimplex2;
		}
	case EDomainWarpBPType::DomainWarpType_OpenSimplex2Reduced:
		{
			return FastNoiseLite::DomainWarpType_OpenSimplex2Reduced;
		}
	}
	return FastNoiseLite::DomainWarpType_BasicGrid;
}
