#pragma once

enum class EDrawThreadMethod : uint8;
class UFastNoiseObject;

class FDrawRenderTargetThread : public FRunnable
{
	FRunnableThread* ExecThread = nullptr;
	TWeakObjectPtr<UFastNoiseObject> FastNoiseObject = nullptr;
	TWeakObjectPtr<UTextureRenderTarget2D> RenderTarget = nullptr;
	TWeakObjectPtr<UCanvas> Canvas = nullptr;
	FTextureRenderTargetResource* RenderTargetResource = nullptr;
	double start_time = 0.f;
	EDrawThreadMethod DrawMethod;
	float Thickness = 0.f;
	void Draw1();
	void Draw2();
public:
	
	FDrawRenderTargetThread(TWeakObjectPtr<UFastNoiseObject> NoiseObject,TWeakObjectPtr<UTextureRenderTarget2D> RT,
		TWeakObjectPtr<UCanvas> InCanvas,
		// FTextureRenderTargetResource* InRenderTargetResource
		EDrawThreadMethod InDrawThreadMethod,float InThickness
		);
	
	virtual void Exit() override;
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	virtual ~FDrawRenderTargetThread() override;
	
	
};
