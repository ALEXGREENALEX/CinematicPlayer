// Copyright 2023 - 2026 Alex Zelenskyi. All Rights Reserved.

#pragma once

#include <Engine/CancellableAsyncAction.h>
#include <UObject/UObjectArray.h>
#include "PlayCinematicAsync.generated.h"

struct FStreamableHandle;

class ACinematicPlayerContent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneAsyncResult);

UCLASS(BlueprintType, Meta = (ExposedAsyncProxy = "AsyncAction"))
class CINEMATICPLAYER_API UPlayCinematicAsync : public UCancellableAsyncAction, public FUObjectArray::FUObjectDeleteListener
{
	GENERATED_BODY()

public:
	/**
	 * Allow to play Cinematic Content like Movies and Level Sequences.
	 * 
	 * @param WorldContext - Object that we can obtain a world context from.
	 * @param PlayerController - The player controller on which to play the Cinematic.
	 * @param Content - Cinematic content to play (usually with Sequence or Movie).
	 * @param bAutoDestroy - Try to destroy Automatically with WorldContext (AsyncAction Outer).
	 */
	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", DisplayName = "Play Cinematic", Meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContext", AdvancedDisplay = "3"))
	static UPlayCinematicAsync* PlayCinematic(UObject* WorldContext, APlayerController* PlayerController, TSubclassOf<ACinematicPlayerContent> Content,
		bool bAutoDestroy = true);

	/**
	 * Allow to play Cinematic Content like Movies and Level Sequences Async.
	 * 
	 * @param WorldContext - Object that we can obtain a world context from.
	 * @param PlayerController - The player controller on which to play the Cinematic.
	 * @param Content - Cinematic content to play (usually with Sequence or Movie).
	 * @param AsyncLoadPriority - Priority to pass to the streaming system, higher priority will be loaded first.
	 * @param bAutoDestroy - Try to destroy Automatically with WorldContext (AsyncAction Outer).
	 */
	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", DisplayName = "Play Cinematic Async", Meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContext", AdvancedDisplay = "3"))
	static UPlayCinematicAsync* PlayCinematicAsync(UObject* WorldContext, APlayerController* PlayerController, TSoftClassPtr<ACinematicPlayerContent> Content,
		int32 AsyncLoadPriority = 100, bool bAutoDestroy = true);

	// Begin UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	virtual void BeginDestroy() override;
	// End UBlueprintAsyncActionBase interface

	// Begin FUObjectDeleteListener Interface
	virtual void NotifyUObjectDeleted(const UObjectBase* Object, int32 Index) override;
	virtual void OnUObjectArrayShutdown() override;
	// End FUObjectDeleteListener Interface

#pragma region PlayableContent
	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	ACinematicPlayerContent* GetPlayableContent() const { return PlayableContent.Get(); }

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	APlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	UUserWidget* GetPlayerWidget() const;

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", DisplayName = "Stop (Skip)")
	virtual void Stop();

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer")
	virtual void Pause();

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer")
	virtual void Resume();
#pragma endregion PlayableContent

protected:
	void SpawnCinematicPlayerContent(const TSubclassOf<ACinematicPlayerContent> ContentClass);

private:
	UFUNCTION()
	void StartCallback();

	UFUNCTION()
	void StopCallback();

	UFUNCTION()
	void FinishCallback();

public:
	// A delegate that is invoked once after content playback will started.
	UPROPERTY(BlueprintAssignable, DisplayName = "Start")
	FPlayCutsceneAsyncResult OnStart;

	// A delegate that is invoked when playback Stopped (or Skipped) and can't Finished.
	UPROPERTY(BlueprintAssignable, DisplayName = "Stop")
	FPlayCutsceneAsyncResult OnStop;

	// A delegate that is invoked when playback has reached the end.
	UPROPERTY(BlueprintAssignable, DisplayName = "Finish")
	FPlayCutsceneAsyncResult OnFinish;

private:
	TWeakObjectPtr<APlayerController> PlayerController;
	TSoftClassPtr<ACinematicPlayerContent> ContentSoftClass;
	bool bAutoDestroy = true;
	int32 AsyncLoadPriority = 100;

	const UObjectBase* AutoDestroyOwnerPtr = nullptr;
	TSharedPtr<FStreamableHandle> StreamingHandle;
	TWeakObjectPtr<ACinematicPlayerContent> PlayableContent;
};
