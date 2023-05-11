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
	void FinishCallback();

	UFUNCTION()
	void SkipCallback();

public:
	UPROPERTY(Transient)
	ACinematicPlayerContent* PlayableContent;

	UPROPERTY(BlueprintAssignable, DisplayName = "Start")
	FPlayCutsceneAsyncResult OnStart;

	UPROPERTY(BlueprintAssignable, DisplayName = "Finish")
	FPlayCutsceneAsyncResult OnFinish;

	UPROPERTY(BlueprintAssignable, DisplayName = "Skip")
	FPlayCutsceneAsyncResult OnSkip;

private:
	TWeakObjectPtr<APlayerController> PlayerController;

	UPROPERTY(Transient)
	TSoftClassPtr<ACinematicPlayerContent> ContentSoftClass;
};
