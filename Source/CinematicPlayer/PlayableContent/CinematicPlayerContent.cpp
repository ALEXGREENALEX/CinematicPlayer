#include "CinematicPlayerContent.h"
#include <Blueprint/UserWidget.h>
#include "Logs/CinematicPlayerLogs.h"
#include "Interfaces/CinematicPlayerInterface.h"

void ACinematicPlayerContent::Initialize(APlayerController* PlayerController)
{
	OwningPlayerController = PlayerController;
}

void ACinematicPlayerContent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(OwningPlayerController))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: OwningPlayerController is Not Valid!"), FUNC_STR);
		Destroy();
		return;
	}

	CreatePlayerWidget();
	EnableInput(OwningPlayerController); // Enable receive Input from PlayerController
	EnableInputMapping(OwningPlayerController); // Add Input Mapping

	OpenAndPlayContent();
}

void ACinematicPlayerContent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(OwningPlayerController))
	{
		DisableInputMapping(OwningPlayerController); // Remove Input Mapping
		DisableInput(OwningPlayerController); // Disable receive Input from PlayerController
	}

	RemovePlayerWidget();

	Super::EndPlay(EndPlayReason);

	OwningPlayerController = nullptr;
}

void ACinematicPlayerContent::PlaybackStarted()
{
	ReceiveOnStart();

	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
	}

	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerInterface::Execute_Start(PlayerWidget);
	}
}

void ACinematicPlayerContent::StopAndDestroy()
{
	ReceiveOnStop();

	if (OnStop.IsBound())
	{
		OnStop.Broadcast();
	}

	Destroy();
}

void ACinematicPlayerContent::FinishAndDestroy()
{
	ReceiveOnFinish();

	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast();
	}

	Destroy();
}

void ACinematicPlayerContent::PressAnyKey(bool bPressed)
{
	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerInterface::Execute_AnyKeyPressed(PlayerWidget, bPressed);
	}
}

void ACinematicPlayerContent::PressSkipKey(bool bPressed)
{
	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerInterface::Execute_SkipKeyPressed(PlayerWidget, bPressed);
	}
}

void ACinematicPlayerContent::CreatePlayerWidget()
{
	if (!IsValid(OwningPlayerController))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: OwningPlayerController is Not Valid!"), FUNC_STR);
		return;
	}

	if (!WidgetClass->ImplementsInterface(UCinematicPlayerInterface::StaticClass()))
	{
		UE_LOG(LogCinematicPlayerContent, Warning, TEXT("%s :: WidgetClass class must implement CinematicPlayerInterface!"), FUNC_STR);
		return;
	}

	PlayerWidget = CreateWidget(OwningPlayerController, WidgetClass);
	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerInterface::Execute_Initialize(PlayerWidget, this);
		PlayerWidget->AddToViewport(WidgetsZOrder);
	}
}

void ACinematicPlayerContent::RemovePlayerWidget()
{
	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerInterface::Execute_HideAndDestroy(PlayerWidget);
	}

	PlayerWidget = nullptr;
}
