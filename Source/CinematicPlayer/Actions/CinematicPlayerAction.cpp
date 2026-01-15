// Copyright 2023 - 2026 Alex Zelenskyi. All Rights Reserved.

#include "CinematicPlayerAction.h"
#include "Data/CinematicDataValidationContext.h"
#include "PlayableContent/CinematicPlayerContent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerAction)

UWorld* UCinematicPlayerAction::GetWorld() const
{
	return OwningPlayerContent.IsValid() ? OwningPlayerContent->GetWorld() : nullptr;
}

#pragma region Data Validation
void UCinematicPlayerAction::LogAddError(FCinematicDataValidationContext& Context, const FString& PropertyPath, const FText& Message) const
{
	Context.AddError(Message, PropertyPath);
}

void UCinematicPlayerAction::LogAddWarning(FCinematicDataValidationContext& Context, const FString& PropertyPath, const FText& Message) const
{
	Context.AddWarning(Message, PropertyPath);
}

void UCinematicPlayerAction::LogAddInfoNote(FCinematicDataValidationContext& Context, const FString& PropertyPath, const FText& Message) const
{
	Context.AddInfoNote(Message, PropertyPath);
}
#pragma endregion Data Validation

void UCinematicPlayerAction::Initialize(ACinematicPlayerContent* CinematicPlayerContent)
{
	OwningPlayerContent = CinematicPlayerContent;
}

void UCinematicPlayerAction::Construct()
{
	ReceiveConstruct();
}

void UCinematicPlayerAction::Destruct()
{
	ReceiveDestruct();

	OwningPlayerContent.Reset();
	MarkAsGarbage();
}

void UCinematicPlayerAction::ExecuteAction()
{
	bActionExecuted = false; // Reset flag

	Execute();

	if (!IsAsync())
	{
		ActionExecutionFinished(true);
	}
}

ACinematicPlayerContent* UCinematicPlayerAction::GetOwningContent() const
{
	return OwningPlayerContent.Get();
}

APlayerController* UCinematicPlayerAction::GetPlayerController() const
{
	return OwningPlayerContent.IsValid() ? OwningPlayerContent->GetPlayerController() : nullptr;
}

UUserWidget* UCinematicPlayerAction::GetPlayerWidget() const
{
	return OwningPlayerContent.IsValid() ? OwningPlayerContent->GetPlayerWidget() : nullptr;
}

void UCinematicPlayerAction::FinishAsyncExecution()
{
	if (!bActionExecuted)
	{
		ActionExecutionFinished(true);
	}
}

void UCinematicPlayerAction::ActionExecutionFinished(bool bExecutionResult)
{
	if (!bActionExecuted)
	{
		bActionExecuted = true;
		OnActionExecutionFinished.ExecuteIfBound(this);
	}
}
