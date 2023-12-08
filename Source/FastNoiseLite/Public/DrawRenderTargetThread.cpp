#include "DrawRenderTargetThread.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "FastNoiseObject.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"

FDrawRenderTargetThread::FDrawRenderTargetThread(TWeakObjectPtr<UFastNoiseObject> NoiseObject,
	TWeakObjectPtr<UTextureRenderTarget2D> RT,TWeakObjectPtr<UCanvas> InCanvas
	// FTextureRenderTargetResource* InRenderTargetResource
	)
{
	FastNoiseObject = NoiseObject;
	RenderTarget = RT;
	Canvas = InCanvas;
	// RenderTargetResource = InRenderTargetResource;

	// AsyncTask(ENamedThreads::GameThread,[]()
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Game Thread Id:%d"),FPlatformTLS::GetCurrentThreadId());
	// });
	
	ExecThread = FRunnableThread::Create(this,TEXT("DrawRenderTargetFromNoiseData"));
	
}

void FDrawRenderTargetThread::Exit()
{
	delete this;
}

bool FDrawRenderTargetThread::Init()
{
	start_time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	return true;
}

uint32 FDrawRenderTargetThread::Run()
{

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

	double time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Exit]cost %fms time to wait RenderTargetResource"),time - start_time);
	start_time = time;

	
	if (!RenderTarget.IsValid())
	{
		return -1;
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

	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Run]start draw"));
	
	for (int32 i = 0; i < X; ++i)
	{
		for (int32 j = 0; j < Y; ++j)
		{
			BoxItem.Position = FVector2d(i, j);
			noise_value = FastNoiseObject->NoiseLite.GetNoise<float>(i, j);
			BoxItem.SetColor(FLinearColor(noise_value, noise_value, noise_value, 1.f));
			Canvas->DrawItem(BoxItem);
		}
	}

	
	time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Exit]cost %fms time to fill data"),time - start_time);
	start_time = time;

	
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Run]Canvas->Canvas->GetRenderTarget():%s"),Canvas->Canvas->GetRenderTarget()?TEXT("TRUE"):TEXT("FALSE"));
	
	if (Canvas->Canvas->GetRenderTarget())
	{
		Canvas->Canvas->Flush_GameThread(true);
	}
	
	time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Exit]cost %fms time to flush render target"),time - start_time);

	

	ENQUEUE_RENDER_COMMAND(FlushDeferredResourceUpdateCommand)(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);
		});

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

