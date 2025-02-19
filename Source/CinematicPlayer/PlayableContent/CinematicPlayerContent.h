// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <EnhancedInputSubsystemInterface.h>
#include "CinematicPlayerContent.generated.h"

struct FCinematicDataValidationContainer;

class APlayerController;
class UCinematicPlayerAction;
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayCutsceneResult);

/**
 * CinematicPlayer playable content, must be used only by PlayCinematicAsync function (UPlayCinematicAsync).
 * Don't use directly or call Initialize first!
 */
UCLASS(Abstract, HideDropdown, NotPlaceable, NotBlueprintable, AutoExpandCategories=("CinematicPlayer", "Actions"), HideCategories = ("Collision", "Physics"))
class CINEMATICPLAYER_API ACinematicPlayerContent : public AActor
{
	GENERATED_BODY()

public:
	ACinematicPlayerContent(const FObjectInitializer& ObjectInitializer);

	virtual void Initialize(TWeakObjectPtr<APlayerController> PlayerController);

#pragma region Data Validation
#if WITH_EDITOR
public:
	// Begin UObject overrides
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	// End UObject overrides

protected:
	/**
	 * Check for invalid data, types and settings.
	 * @param DataValidationContainer Container for errors and warnings.
	 */
	virtual void ValidateData(FCinematicDataValidationContainer& DataValidationContainer) const;
#endif
#pragma endregion Data Validation

protected:
	// Begin AActor overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor overrides

	virtual void OpenAndPlayContent() { ; }
	virtual void RequestDestroy();

	virtual void EnableInputAndCreateUI();
	virtual void DisableInputAndRemoveUI();

public:
	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	APlayerController* GetPlayerController() const { return OwningPlayerController.Get(); }

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer")
	UUserWidget* GetPlayerWidget() const { return PlayerUserWidget.Get(); }

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
	virtual void PlaybackStarted();
	virtual void PlaybackStopped();
	virtual void PlaybackFinished();

	void ExecuteActionsAsync(const TArray<TObjectPtr<UCinematicPlayerAction>>& Actions, int32 ActionIndex, TFunction<void()> Callback);
	void ForEachCinematicPlayerAction(const TFunctionRef<void(UCinematicPlayerAction* CinematicPlayerAction)>& Predicate) const;

#pragma region Blueprint Events
	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayer", DisplayName = "OnStart")
	void ReceiveOnStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayer", DisplayName = "OnStop")
	void ReceiveOnStop();

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayer", DisplayName = "OnFinish")
	void ReceiveOnFinish();

	UFUNCTION(BlueprintNativeEvent, Category = "Input")
	void AddInputMapping();
	virtual void AddInputMapping_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Input")
	void RemoveInputMapping();
	virtual void RemoveInputMapping_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "UserInterface")
	UUserWidget* CreatePlayerWidget(APlayerController* PlayerController);
	virtual UUserWidget* CreatePlayerWidget_Implementation(APlayerController* PlayerController);

	UFUNCTION(BlueprintNativeEvent, Category = "UserInterface")
	void RemovePlayerWidget(UUserWidget* PlayerWidget);
	virtual void RemovePlayerWidget_Implementation(UUserWidget* PlayerWidget);
#pragma endregion Blueprint Events

public:
	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnStart;

	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnStop;

	UPROPERTY(BlueprintAssignable)
	FPlayCutsceneResult OnFinish;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	bool bCanSkip = true;

	// Called before Content playback (before OnStart delegate).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", Instanced)
	TArray<TObjectPtr<UCinematicPlayerAction>> StartupActions;

	// Called when Stop (Skip) function called or error occured, but before OnStop delegate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", Instanced)
	TArray<TObjectPtr<UCinematicPlayerAction>> StopActions;

	// Called when Stop (Skip) function called or error occured, after OnStop delegate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", Instanced)
	TArray<TObjectPtr<UCinematicPlayerAction>> PostStopActions;

	// Called when playback finished without errors, but before OnFinish delegate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", Instanced)
	TArray<TObjectPtr<UCinematicPlayerAction>> FinishActions;

	// Called when playback finished without errors, after OnFinish delegate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", Instanced)
	TArray<TObjectPtr<UCinematicPlayerAction>> PostFinishActions;

	// Called after all actions when playback completed.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", Instanced)
	TArray<TObjectPtr<UCinematicPlayerAction>> EndActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 InputMappingPriority = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FModifyContextOptions InputMappingOptions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UserInterface", Meta = (MustImplement = "/Script/CinematicPlayer.CinematicPlayerInterface"))
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UserInterface")
	int32 WidgetsZOrder = 0;

protected:
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputSubsystem;
	TWeakObjectPtr<UUserWidget> PlayerUserWidget;
};
