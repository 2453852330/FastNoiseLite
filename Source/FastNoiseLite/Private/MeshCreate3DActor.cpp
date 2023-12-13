// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshCreate3DActor.h"

#include "FastNoiseLibUtils.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AMeshCreate3DActor::AMeshCreate3DActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComp"));
	RootComponent = InstancedStaticMeshComponent;
}

// Called when the game starts or when spawned
void AMeshCreate3DActor::BeginPlay()
{
	Super::BeginPlay();

	FastNoiseObject = UFastNoiseLibUtils::Lib_CreateFastNoiseObject(this);
	FastNoiseObject->BP_SetNoiseCreateParam(NoiseCreateParams);

	
	if (b2D)
	{
		for (int32 i = 0; i < Size.X; ++i)
		{
			for (int32 j = 0; j < Size.Y; ++j)
			{
				float value = FastNoiseObject->NoiseLite.GetNoise<float>((float)i,(float)j);
				FVector Pos = FVector(i,j,0) * Span;
				Pos.Z = value * HeightScale;
				InstancedStaticMeshComponent->AddInstance(FTransform(Pos));
			}
		}
	}
	else
	{
		for (int32 i = 0; i < Size.X; ++i)
		{
			for (int32 j = 0; j < Size.Y; ++j)
			{
				for (int32 k = 0; k < Size.Z; ++k)
				{
					float value = FastNoiseObject->NoiseLite.GetNoise<float>((float)i,(float)j,(float)k);
					if (value >= CheckValue_3D)
					{
						InstancedStaticMeshComponent->AddInstance(CF_GetTransformFromXYZ(i,j,k,value));
					}
				}
			}
		}
	}
}

FTransform AMeshCreate3DActor::CF_GetTransformFromXYZ(int32 X, int32 Y, int32 Z,float Value)
{
	FTransform Transform;
	Transform.SetRotation(FQuat::Identity);
	Transform.SetScale3D(FVector(1.f));
	
	if (b2D)
	{
		FVector Pos = FVector(X,Y,Z) * Span;
		Pos.Z = Value * HeightScale;
		Transform.SetLocation(Pos);
	}
	else
	{
		Transform.SetLocation(FVector(X,Y,Z) * Span);
	}
	
	return Transform;
}
