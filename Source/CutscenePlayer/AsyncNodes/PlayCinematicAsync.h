#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintAsyncActionBase.h>
#include "PlayCinematicAsync.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCinematicPlayer, Log, All);

class ACinematicPlayerContent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneAsyncResult);

UCLASS(Meta = (ExposedAsyncProxy))
class CINEMATICPLAYER_API UPlayCinematicAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// Begin UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	// End UBlueprintAsyncActionBase interface

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", Meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UPlayCinematicAsync* PlayCinematicAsync(UObject* WorldContextObject, APlayerController* PlayerController, TSoftClassPtr<ACinematicPlayerContent> Content);

private:
	UFUNCTION()
	void StartCallback();

	UFUNCTION()
	void StopCallback();

	UFUNCTION()
	void FinishCallback();

public:
	UPROPERTY(Transient)
	ACinematicPlayerContent* PlayableContent;

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

	UPROPERTY(Transient)
	TSoftClassPtr<ACinematicPlayerContent> ContentSoftClass;
};
