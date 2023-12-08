#pragma once

class UFastNoiseObject;

class FDrawRenderTargetThread : public FRunnable
{
	FRunnableThread* ExecThread = nullptr;
	TWeakObjectPtr<UFastNoiseObject> FastNoiseObject = nullptr;
	TWeakObjectPtr<UTextureRenderTarget2D> RenderTarget = nullptr;
	TWeakObjectPtr<UCanvas> Canvas = nullptr;
	FTextureRenderTargetResource* RenderTargetResource = nullptr;
	double start_time = 0.f;
public:
	
	FDrawRenderTargetThread(TWeakObjectPtr<UFastNoiseObject> NoiseObject,TWeakObjectPtr<UTextureRenderTarget2D> RT,
		TWeakObjectPtr<UCanvas> InCanvas
		// FTextureRenderTargetResource* InRenderTargetResource
		);
	
	virtual void Exit() override;
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	virtual ~FDrawRenderTargetThread() override;
	

};
