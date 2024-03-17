#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "CinematicPlayerContent.generated.h"

class APlayerController;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneResult);

// CinematicPlayer playable content, must be used only by PlayCinematicAsync function (UPlayCinematicAsync).
// Don't use directly or call Initialize first!
UCLASS(Abstract, HideDropdown, NotPlaceable, NotBlueprintable)
class CINEMATICPLAYER_API ACinematicPlayerContent : public AActor
{
	GENERATED_BODY()

public:
	virtual void Initialize(TWeakObjectPtr<APlayerController> PlayerController);

	// Begin AActor overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor overrides

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	APlayerController* GetPlayerController() const { return OwningPlayerController.Get(); }

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	UUserWidget* GetPlayerWidget() const { return PlayerWidget.Get(); }

	virtual void OpenAndPlayContent() { ; }
	virtual void PlaybackStarted();
	virtual void StopAndDestroy();
	virtual void FinishAndDestroy();

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", DisplayName = "Stop (Skip)")
	virtual void Stop() { ; }

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer")
	virtual void Pause() { ; }

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer")
	virtual void Resume() { ; }

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer|Input")
	void PressAnyKey(bool bPressed);

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer|Input")
	void PressSkipKey(bool bPressed);

protected:
	virtual void CreatePlayerWidget();
	virtual void RemovePlayerWidget();

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayer|Events")
	void ReceiveOnStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayer|Events")
	void ReceiveOnStop();

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayer|Events")
	void ReceiveOnFinish();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void EnableInputMapping(APlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void DisableInputMapping(APlayerController* PlayerController);

public:
	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnStart;

	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnStop;

	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnFinish;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	bool bCanSkip = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer", AdvancedDisplay, Meta = (MustImplement = "/Script/CinematicPlayer.CinematicPlayerInterface"))
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer", AdvancedDisplay)
	int32 WidgetsZOrder = 0;

protected:
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<UUserWidget> PlayerWidget;
};
