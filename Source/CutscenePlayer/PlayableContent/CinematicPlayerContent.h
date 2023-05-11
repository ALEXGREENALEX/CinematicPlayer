#pragma once

#include <CoreMinimal.h>
#include "CinematicPlayerContent.generated.h"

class APlayerController;
class UUserWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogCinematicPlayerContent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneResult);

// CinematicPlayer playable content, must be used only by PlayCinematicAsync function (UPlayCinematicAsync).
// Don't use directly or call Initialize first!
UCLASS(Abstract, HideDropdown, NotPlaceable, NotBlueprintable)
class CINEMATICPLAYER_API ACinematicPlayerContent : public AActor
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* PlayerController);

	// Begin AActor overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor overrides

	virtual void OpenAndPlayContent() { ; }
	virtual void PlaybackStarted();
	virtual void SkipAndDestroy();
	virtual void FinishAndDestroy();

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer", DisplayName = "Skip (Stop)")
	virtual void Skip() { ; }

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer")
	virtual void Pause();

	UFUNCTION(BlueprintCallable, Category = "CinematicPlayer")
	virtual void Resume();

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
	void ReceiveOnSkip();

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
	FPlayCutsceneResult OnFinish;

	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnSkip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	bool bCanSkip = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer", AdvancedDisplay, Meta = (MustImplement = CinematicPlayerInterface))
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer", AdvancedDisplay)
	int32 WidgetsZOrder = 0;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CinematicPlayer")
	APlayerController* OwningPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "CinematicPlayer")
	UUserWidget* PlayerWidget;
};
