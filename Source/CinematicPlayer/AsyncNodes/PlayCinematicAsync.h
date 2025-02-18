// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintAsyncActionBase.h>
#include "PlayCinematicAsync.generated.h"

class ACinematicPlayerContent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneAsyncResult);

UCLASS(Meta = (ExposedAsyncProxy))
class CINEMATICPLAYER_API UPlayCinematicAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// Allow to play Cinematic Content like Movies and Level Sequences Async.
	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", Meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UPlayCinematicAsync* PlayCinematicAsync(UObject* WorldContextObject, APlayerController* PlayerController, TSoftClassPtr<ACinematicPlayerContent> Content);

	// Begin UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	// End UBlueprintAsyncActionBase interface

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
	TWeakObjectPtr<ACinematicPlayerContent> PlayableContent;
};
