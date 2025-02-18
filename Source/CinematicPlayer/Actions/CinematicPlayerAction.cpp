// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "CinematicPlayerAction.h"
#include "PlayableContent/CinematicPlayerContent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerAction)

UWorld* UCinematicPlayerAction::GetWorld() const
{
	return OwningPlayerContent.IsValid() ? OwningPlayerContent->GetWorld() : nullptr;
}

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

ACinematicPlayerContent* UCinematicPlayerAction::GetPlayerContent() const
{
	return OwningPlayerContent.Get();
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
