#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <EnhancedInputSubsystemInterface.h>
#include "CinematicPlayerContent.generated.h"

class APlayerController;
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneResult);

// CinematicPlayer playable content, must be used only by PlayCinematicAsync function (UPlayCinematicAsync).
// Don't use directly or call Initialize first!
UCLASS(Abstract, HideDropdown, NotPlaceable, NotBlueprintable)
class CINEMATICPLAYER_API ACinematicPlayerContent : public AActor
{
	GENERATED_BODY()

public:
	ACinematicPlayerContent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

	UFUNCTION(BlueprintNativeEvent, Category = "CinematicPlayer|Input")
	void AddInputMapping();
	virtual void AddInputMapping_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "CinematicPlayer|Input")
	void RemoveInputMapping();
	virtual void RemoveInputMapping_Implementation();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 InputMappingPriority = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FModifyContextOptions InputMappingOptions;

protected:
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputSubsystem;
	TWeakObjectPtr<UUserWidget> PlayerWidget;
};
