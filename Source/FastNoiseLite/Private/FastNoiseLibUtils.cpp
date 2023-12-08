// Fill out your copyright notice in the Description page of Project Settings.


#include "FastNoiseLibUtils.h"

UFastNoiseObject* UFastNoiseLibUtils::Lib_CreateFastNoiseObject(UObject* Owner)
{
	return NewObject<UFastNoiseObject>(Owner);
}
