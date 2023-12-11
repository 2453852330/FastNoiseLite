#include "DrawRenderTargetThread.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "FastNoiseObject.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

void FDrawRenderTargetThread::Draw1()
{

	if (!RenderTarget.IsValid())
	{
		return;
	}
	
	AsyncTask(ENamedThreads::RHIThread,[this]()
	{
		this->RenderTargetResource = RenderTarget->GetRenderTargetResource();
	});

	// RenderTargetResource = RenderTarget->GetRenderTargetResource();
	
	while (!RenderTargetResource)
	{
		// UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Run]wait render target resource"));
		FPlatformProcess::Sleep(0.01f);
	}



	int32 X = RenderTarget->SizeX;
	int32 Y = RenderTarget->SizeY;

	
	UWorld* World = FastNoiseObject->GetWorld();
	World->FlushDeferredParameterCollectionInstanceUpdates();

	FCanvas* NewCanvas = new FCanvas(
		RenderTargetResource,
		nullptr,
		World,
		World->GetFeatureLevel(),
		// Draw immediately so that interleaved SetVectorParameter (etc) function calls work as expected
		FCanvas::CDM_DeferDrawing);

	
	
	Canvas->Init(X, Y, nullptr, NewCanvas);
	
	
	float noise_value = 0.f;
	FCanvasBoxItem BoxItem(FVector2d::ZeroVector,FVector2d(1.f));
	BoxItem.LineThickness = 1.f;


	
	for (int32 i = 0; i < X; ++i)
	{
		for (int32 j = 0; j < Y; ++j)
		{
			BoxItem.Position = FVector2d(i, j);
			noise_value = FastNoiseObject->NoiseLite.GetNoise<float>(i, j);
			BoxItem.SetColor(FLinearColor(noise_value, noise_value, noise_value, 1.f));
			BoxItem.Draw(NewCanvas);
		}
	}

	

	

	ENQUEUE_RENDER_COMMAND(FlushDeferredResourceUpdateCommand)(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			Canvas->Canvas->Flush_GameThread(true);
			RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);
			UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Draw1]cost %f ms"),FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - start_time);
			delete this;
		});
}

void FDrawRenderTargetThread::Draw2()
{
	if (!RenderTarget.IsValid())
	{
		return;
	}

	AsyncTask(ENamedThreads::RHIThread,[this]()
		{
			// this->RenderTargetResource = RenderTarget->GetRenderTargetResource();a
			// UKismetRenderingLibrary::ClearRenderTarget2D(FastNoiseObject.Get(),RenderTarget.Get());
			RenderTargetResource = RenderTarget->GetRenderTargetResource();
		});
	
	while (!RenderTargetResource)
	{
		FPlatformProcess::Sleep(0.01f);
	}
	

	
	int32 SizeX = RenderTarget->SizeX;
	int32 SizeY = RenderTarget->SizeY;
	
	
	UWorld* World = FastNoiseObject->GetWorld();
	World->FlushDeferredParameterCollectionInstanceUpdates();

	
	FCanvas* NewCanvas = new FCanvas(RenderTargetResource,nullptr, World,World->GetFeatureLevel(),
			// Draw immediately so that interleaved SetVectorParameter (etc) function calls work as expected
			FCanvas::CDM_DeferDrawing);

	// init the UCanvas use FCanvas
	Canvas->Init(SizeX, SizeY, nullptr, NewCanvas);


	float color_value = 0.f;
	
	FBatchedElements* BatchedElements = Canvas->Canvas->GetBatchedElements(FCanvas::ET_Line);
	FHitProxyId HitProxyId = Canvas->Canvas->GetHitProxyId();

	for (int32 i = 0; i < SizeX; ++i)
	{
		for (int32 j = 0; j < SizeY; ++j)
		{
			color_value = FastNoiseObject->NoiseLite.GetNoise<float>(i,j);
			// BoxItem.Position.X = i;
			// BoxItem.Position.Y = j;
			// draw_color.G = color_value;
			// BoxItem.SetColor(draw_color);
			// Canvas->DrawItem(BoxItem);
			// BatchedElements->AddPoint(FVector(i,j,0.f),50.f,FLinearColor(0.f,color_value,0.f),HitProxyId);

			BatchedElements->AddLine(FVector(i,j,0.f),FVector(i,j,0.f),FLinearColor(0.f,color_value,0.f),HitProxyId,Thickness);
		}
	}

	

	ENQUEUE_RENDER_COMMAND(FlushDeferredResourceUpdateCommand)(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			Canvas->Canvas->Flush_GameThread(true);
			RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);
			UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Draw2]cost %f ms"),FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - start_time);
			delete this;
		});
}



FDrawRenderTargetThread::FDrawRenderTargetThread(TWeakObjectPtr<UFastNoiseObject> NoiseObject,
                                                 TWeakObjectPtr<UTextureRenderTarget2D> RT,TWeakObjectPtr<UCanvas> InCanvas,
                                                 // FTextureRenderTargetResource* InRenderTargetResource,
                                                 EDrawThreadMethod InDrawThreadMethod,float InThickness
)
{
	FastNoiseObject = NoiseObject;
	RenderTarget = RT;
	Canvas = InCanvas;
	DrawMethod = InDrawThreadMethod;
	Thickness = InThickness;
	// RenderTargetResource = InRenderTargetResource;

	// AsyncTask(ENamedThreads::GameThread,[]()
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Game Thread Id:%d"),FPlatformTLS::GetCurrentThreadId());
	// });

	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->FDrawRenderTargetThread] start draw thread"));
	
	ExecThread = FRunnableThread::Create(this,TEXT("DrawRenderTargetFromNoiseData"));
	
}

void FDrawRenderTargetThread::Exit()
{
	
}


bool FDrawRenderTargetThread::Init()
{
	start_time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	return true;
}

uint32 FDrawRenderTargetThread::Run()
{
	// Draw1();
	switch (DrawMethod)
	{
	case EDrawThreadMethod::EMethod1:
		{
			Draw1();
			break;
		}
	case EDrawThreadMethod::EMethod2:
		{
			Draw2();
			break;
		}
	}
	
	return 1;
}

void FDrawRenderTargetThread::Stop()
{
}

FDrawRenderTargetThread::~FDrawRenderTargetThread()
{
	if (ExecThread)
	{
		ExecThread->Kill();
		delete ExecThread;
	}
}

