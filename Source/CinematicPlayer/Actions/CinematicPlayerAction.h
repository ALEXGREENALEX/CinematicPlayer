// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>
#include "CinematicPlayerAction.generated.h"

class ACinematicPlayerContent;
class APlayerController;
class UUserWidget;

/**
 * CinematicPlayer Async Action.
 * Used in CinematicPlayerContent to add Async/Sync logic before/after playback.
 */
UCLASS(Abstract, HideDropdown, HideCategories = ("Hidden"), BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class CINEMATICPLAYER_API UCinematicPlayerAction : public UObject
{
	GENERATED_BODY()

public:
	// Begin UObject overrides
	virtual UWorld* GetWorld() const override;
	// End UObject overrides

	virtual void Initialize(ACinematicPlayerContent* CinematicPlayerContent);
	virtual void Construct();
	virtual void Destruct();
	virtual void ExecuteAction();

	UFUNCTION(BlueprintPure, Category = "CinematicPlayerAction")
	ACinematicPlayerContent* GetOwningContent() const;

	UFUNCTION(BlueprintPure, Category = "CinematicPlayerAction")
	APlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "CinematicPlayerAction")
	UUserWidget* GetPlayerWidget() const;

protected:
	// Need to be called every time, when Async Quest Action executed!
	UFUNCTION(BlueprintCallable, Category = "CinematicPlayerAction")
	virtual void FinishAsyncExecution();

	virtual void ActionExecutionFinished(bool bExecutionResult);

#pragma region Blueprint Events
public:
	/**
	 * Allow to make Actions Async.
	 * Must call FinishAsyncExecution() when action finished!
	 * @return true if Async, false by default.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CinematicPlayer Action")
	bool IsAsync() const;
	virtual bool IsAsync_Implementation() const { return false; }

protected:
	// Execute Action
	UFUNCTION(BlueprintNativeEvent, Category = "CinematicPlayer Action")
	void Execute();
	virtual void Execute_Implementation() { ; }

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayerAction", DisplayName = "Construct")
	void ReceiveConstruct();

	UFUNCTION(BlueprintImplementableEvent, Category = "CinematicPlayerAction", DisplayName = "Destruct")
	void ReceiveDestruct();
#pragma endregion Blueprint Events

public:
#pragma region Delegates
	DECLARE_DELEGATE_OneParam(FCinematicPlayerActionDelegate, UCinematicPlayerAction* /*CinematicPlayerAction*/)

	// Called when ActionExecutionFinished for continue another actions execution when this one will Complete.
	FCinematicPlayerActionDelegate OnActionExecutionFinished;
#pragma endregion Delegates

protected:
	TWeakObjectPtr<ACinematicPlayerContent> OwningPlayerContent;

	bool bActionExecuted = false;
};
