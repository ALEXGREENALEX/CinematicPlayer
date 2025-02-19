// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "CinematicPlayerContent.h"
#include <Blueprint/UserWidget.h>
#include <EnhancedInputSubsystems.h>
#include <InputMappingContext.h>
#include "Logs/CinematicPlayerLogs.h"
#include "Actions/CinematicPlayerAction.h"
#include "Data/CinematicDataValidationContainer.h"
#include "Interfaces/CinematicPlayerWidgetInterface.h"

#if WITH_EDITOR
#include <AssetRegistry/AssetData.h>
#include <Misc/DataValidation.h>
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerContent)

ACinematicPlayerContent::ACinematicPlayerContent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputMappingOptions.bIgnoreAllPressedKeysUntilRelease = true;
	InputMappingOptions.bForceImmediately = true; // Need update all before Spawn Widget Hint
}

void ACinematicPlayerContent::Initialize(TWeakObjectPtr<APlayerController> PlayerController)
{
	OwningPlayerController = PlayerController;

	ForEachCinematicPlayerAction([this](UCinematicPlayerAction* CinematicPlayerAction)
	{
		if (IsValid(CinematicPlayerAction))
		{
			CinematicPlayerAction->Initialize(this);
		}
	});
}

#pragma region Data Validation
#if WITH_EDITOR
EDataValidationResult ACinematicPlayerContent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	const UClass* Class = GetClass();
	if (!IsValid(Class) || Class->HasAnyClassFlags(CLASS_Abstract))
	{
		return Result; // Allow to mark Base BP as abstract and skip validation and errors
	}

	// Suspend errors when Saving
	if (Context.GetValidationUsecase() == EDataValidationUsecase::Save)
	{
		return Result;
	}

	FCinematicDataValidationContainer Container;
	ValidateData(Container);

	// Hide full path to self in Compile Log (UseCase == None), but show with Manual Validation.
	const UObject* Blueprint = GetClass() ? GetClass()->ClassGeneratedBy : nullptr;
	const FAssetData Asset = FAssetData(Context.GetValidationUsecase() == EDataValidationUsecase::None ? nullptr : Blueprint);

	bool bHasAnyErrorsOrWarnings = false;
	for (const FCinematicDataValidationRecord& Record : Container.Records)
	{
		if (Record.PropertyPath.IsEmpty())
		{
			Context.AddMessage(Asset, Record.Severity, Record.Message);
		}
		else
		{
			const FText MessageToLog = FText::Format(INVTEXT("{0}: {1}"), FText::FromString(Record.PropertyPath), Record.Message);
			Context.AddMessage(Asset, Record.Severity, MessageToLog);
		}

		if (Record.Severity <= EMessageSeverity::Info)
		{
			bHasAnyErrorsOrWarnings = true;
		}
	}

	if (bHasAnyErrorsOrWarnings)
	{
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void ACinematicPlayerContent::ValidateData(FCinematicDataValidationContainer& DataValidationContainer) const
{
	// No need in all cases, but need for current project
	if (bCanSkip && !IsValid(InputMappingContext))
	{
		DataValidationContainer.AddWarning(INVTEXT("CanSkip = true, but InputMappingContext is Not Valid or Empty!"));
	}

	auto ValidateActions = [&](const TArray<TObjectPtr<UCinematicPlayerAction>>& Actions, const FString& PropertyName)
	{
		for (int32 i = 0; i < Actions.Num(); i++)
		{
			const FString PropertyPath = FString::Format(TEXT("{0}[{1}]"), {PropertyName, i});
			UCinematicPlayerAction* Action = Actions[i];
			if (IsValid(Action))
			{
				Action->ValidateData(DataValidationContainer, PropertyPath);
			}
			else
			{
				DataValidationContainer.AddWarning(INVTEXT("Action is Not Valid or Empty!"), PropertyPath);
			}
		}
	};

	ValidateActions(StartupActions, GET_MEMBER_NAME_STRING_CHECKED(ThisClass, StartupActions));
	ValidateActions(StopActions, GET_MEMBER_NAME_STRING_CHECKED(ThisClass, StopActions));
	ValidateActions(PostStopActions, GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PostStopActions));
	ValidateActions(FinishActions, GET_MEMBER_NAME_STRING_CHECKED(ThisClass, FinishActions));
	ValidateActions(PostFinishActions, GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PostFinishActions));
	ValidateActions(EndActions, GET_MEMBER_NAME_STRING_CHECKED(ThisClass, EndActions));
}
#endif
#pragma endregion Data Validation

void ACinematicPlayerContent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwningPlayerController.IsValid())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] OwningPlayerController is Not Valid!", FUNC_STR);
		Destroy();
		return;
	}

	ForEachCinematicPlayerAction([](UCinematicPlayerAction* CinematicPlayerAction)
	{
		if (IsValid(CinematicPlayerAction))
		{
			CinematicPlayerAction->Construct();
		}
	});

	ExecuteActionsAsync(StartupActions, 0, [this]()
	{
		EnableInputAndCreateUI();
		OpenAndPlayContent();
	});
}

void ACinematicPlayerContent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DisableInputAndRemoveUI();

	ForEachCinematicPlayerAction([](UCinematicPlayerAction* CinematicPlayerAction)
	{
		if (IsValid(CinematicPlayerAction))
		{
			CinematicPlayerAction->OnActionExecutionFinished.Unbind();
			CinematicPlayerAction->Destruct();
			CinematicPlayerAction->MarkAsGarbage();
		}
	});

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerContent::RequestDestroy()
{
	Destroy(false, false);
}

void ACinematicPlayerContent::EnableInputAndCreateUI()
{
	EnableInput(OwningPlayerController.Get()); // Enable receive Input from PlayerController
	AddInputMapping();

	PlayerUserWidget = CreatePlayerWidget(OwningPlayerController.Get());
}

void ACinematicPlayerContent::DisableInputAndRemoveUI()
{
	RemoveInputMapping();

	if (OwningPlayerController.IsValid())
	{
		DisableInput(OwningPlayerController.Get()); // Disable receive Input from PlayerController
	}

	if (PlayerUserWidget.IsValid())
	{
		RemovePlayerWidget(PlayerUserWidget.Get());
		PlayerUserWidget.Reset();
	}
}

void ACinematicPlayerContent::PressAnyKey(bool bPressed)
{
	if (PlayerUserWidget.IsValid())
	{
		ICinematicPlayerWidgetInterface::Execute_AnyKeyPressed(PlayerUserWidget.Get(), bPressed);
	}
}

void ACinematicPlayerContent::PressSkipKey(bool bPressed)
{
	if (PlayerUserWidget.IsValid())
	{
		ICinematicPlayerWidgetInterface::Execute_SkipKeyPressed(PlayerUserWidget.Get(), bPressed);
	}
}

void ACinematicPlayerContent::PlaybackStarted()
{
	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
	}

	ReceiveOnStart();

	if (PlayerUserWidget.IsValid())
	{
		ICinematicPlayerWidgetInterface::Execute_Start(PlayerUserWidget.Get());
	}
}

void ACinematicPlayerContent::PlaybackStopped()
{
	DisableInputAndRemoveUI();

	ExecuteActionsAsync(StopActions, 0, [this]()
	{
		ReceiveOnStop();

		if (OnStop.IsBound())
		{
			OnStop.Broadcast();
		}

		ExecuteActionsAsync(PostStopActions, 0, [this]()
		{
			ExecuteActionsAsync(EndActions, 0, [this]()
			{
				RequestDestroy();
			});
		});
	});
}

void ACinematicPlayerContent::PlaybackFinished()
{
	DisableInputAndRemoveUI();

	ExecuteActionsAsync(FinishActions, 0, [this]()
	{
		ReceiveOnFinish();

		if (OnFinish.IsBound())
		{
			OnFinish.Broadcast();
		}

		ExecuteActionsAsync(PostFinishActions, 0, [this]()
		{
			ExecuteActionsAsync(EndActions, 0, [this]()
			{
				RequestDestroy();
			});
		});
	});
}

void ACinematicPlayerContent::ExecuteActionsAsync(const TArray<TObjectPtr<UCinematicPlayerAction>>& Actions, int32 ActionIndex, TFunction<void()> Callback)
{
	if (ActionIndex >= Actions.Num())
	{
		Callback();
		return;
	}

	const int32 NextIndex = ActionIndex + 1;

	UCinematicPlayerAction* Action = Actions[ActionIndex];
	if (!IsValid(Action))
	{
		ExecuteActionsAsync(Actions, NextIndex, Callback);
		return;
	}

	Action->OnActionExecutionFinished.BindWeakLambda(this, [&, NextIndex, Callback](UCinematicPlayerAction* FinishedAction)
	{
		if (IsValid(FinishedAction))
		{
			FinishedAction->OnActionExecutionFinished.Unbind();
		}

		ExecuteActionsAsync(Actions, NextIndex, Callback);
	});

	Action->ExecuteAction();
}

void ACinematicPlayerContent::ForEachCinematicPlayerAction(const TFunctionRef<void(UCinematicPlayerAction* CinematicPlayerAction)>& Predicate) const
{
	auto CallPredicate = [&](const TArray<TObjectPtr<UCinematicPlayerAction>>& Actions)
	{
		for (UCinematicPlayerAction* Action : Actions)
		{
			Predicate(Action);
		}
	};

	CallPredicate(StartupActions);
	CallPredicate(StopActions);
	CallPredicate(PostStopActions);
	CallPredicate(FinishActions);
	CallPredicate(PostFinishActions);
	CallPredicate(EndActions);
}

void ACinematicPlayerContent::AddInputMapping_Implementation()
{
	if (!IsValid(InputMappingContext))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] InputMappingContext is Not Valid!", FUNC_STR);
		return;
	}

	if (!OwningPlayerController.IsValid())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] OwningPlayerController is Not Valid!", FUNC_STR);
		return;
	}

	const ULocalPlayer* LocalPlayer = OwningPlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] LocalPlayer is Not Valid!", FUNC_STR);
		return;
	}

	EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!EnhancedInputSubsystem.IsValid())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] EnhancedInputSubsystem is Not Valid!", FUNC_STR);
		return;
	}

	EnhancedInputSubsystem->AddMappingContext(InputMappingContext, InputMappingPriority, InputMappingOptions);
}

void ACinematicPlayerContent::RemoveInputMapping_Implementation()
{
	if (EnhancedInputSubsystem.IsValid())
	{
		EnhancedInputSubsystem->RemoveMappingContext(InputMappingContext, InputMappingOptions);
	}
}

UUserWidget* ACinematicPlayerContent::CreatePlayerWidget_Implementation(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] PlayerController is Not Valid!", FUNC_STR);
		return nullptr;
	}

	if (!IsValid(WidgetClass))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] WidgetClass is Not Valid!", FUNC_STR);
		return nullptr;
	}

	if (!WidgetClass->ImplementsInterface(UCinematicPlayerWidgetInterface::StaticClass()))
	{
		UE_LOGFMT(LogCinematicPlayer, Warning, "[{FUNC}] WidgetClass class must implement CinematicPlayerInterface!", FUNC_STR);
		return nullptr;
	}

	UUserWidget* PlayerWidget = CreateWidget(OwningPlayerController.Get(), WidgetClass);
	if (!IsValid(PlayerWidget))
	{
		UE_LOGFMT(LogCinematicPlayer, Warning, "[{FUNC}] Can't create PlayerWidget!", FUNC_STR);
		return nullptr;
	}

	ICinematicPlayerWidgetInterface::Execute_Initialize(PlayerWidget, bCanSkip, this);
	PlayerWidget->AddToViewport(WidgetsZOrder);
	return PlayerWidget;
}

void ACinematicPlayerContent::RemovePlayerWidget_Implementation(UUserWidget* PlayerWidget)
{
	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerWidgetInterface::Execute_HideAndDestroy(PlayerWidget);
	}
}
