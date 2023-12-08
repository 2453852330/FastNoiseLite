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

	AsyncTask(ENamedThreads::GameThread,[]()
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Thread Id:%d"),FPlatformTLS::GetCurrentThreadId());
	});
	
	ExecThread = FRunnableThread::Create(this,TEXT("DrawRenderTargetFromNoiseData"));
	
}

void FDrawRenderTargetThread::Exit()
{
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Exit]cost %f time"),FPlatformTime::Seconds() - start_time);
	delete this;
}

bool FDrawRenderTargetThread::Init()
{
	start_time = FPlatformTime::Seconds();
	return true;
}

uint32 FDrawRenderTargetThread::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("FDrawRenderTargetThread Id:%d"),FPlatformTLS::GetCurrentThreadId());

	AsyncTask(ENamedThreads::RHIThread,[this]()
	{
		this->RenderTargetResource = RenderTarget->GetRenderTargetResource();
	});

	
	while (!RenderTargetResource)
	{
		// UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Run]wait render target resource"));
		FPlatformProcess::Sleep(0.01f);
	}
	
	
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
		FCanvas::CDM_ImmediateDrawing);

	
	
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
	
	

	
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->Run]draw finish"));

	ENQUEUE_RENDER_COMMAND(FlushDeferredResourceUpdateCommand)(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->operator()]Flush in RHI"));
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
	UE_LOG(LogTemp, Warning, TEXT("[DrawRenderTargetThread->~FDrawRenderTargetThread]delete finish"));
}

