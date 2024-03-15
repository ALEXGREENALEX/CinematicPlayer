#include "CinematicPlayerContent.h"
#include <Blueprint/UserWidget.h>
#include "Logs/CinematicPlayerLogs.h"
#include "Interfaces/CinematicPlayerInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerContent)

void ACinematicPlayerContent::Initialize(TWeakObjectPtr<APlayerController> PlayerController)
{
	OwningPlayerController = PlayerController;
}

void ACinematicPlayerContent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwningPlayerController.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: OwningPlayerController is Not Valid!"), FUNC_STR);
		Destroy();
		return;
	}

	CreatePlayerWidget();
	EnableInput(OwningPlayerController.Get()); // Enable receive Input from PlayerController
	EnableInputMapping(OwningPlayerController.Get()); // Add Input Mapping

	OpenAndPlayContent();
}

void ACinematicPlayerContent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwningPlayerController.IsValid())
	{
		DisableInputMapping(OwningPlayerController.Get()); // Remove Input Mapping
		DisableInput(OwningPlayerController.Get()); // Disable receive Input from PlayerController
	}

	RemovePlayerWidget();

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerContent::PlaybackStarted()
{
	ReceiveOnStart();

	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
	}

	if (PlayerWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_Start(PlayerWidget.Get());
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
	if (PlayerWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_AnyKeyPressed(PlayerWidget.Get(), bPressed);
	}
}

void ACinematicPlayerContent::PressSkipKey(bool bPressed)
{
	if (PlayerWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_SkipKeyPressed(PlayerWidget.Get(), bPressed);
	}
}

void ACinematicPlayerContent::CreatePlayerWidget()
{
	if (!OwningPlayerController.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: OwningPlayerController is Not Valid!"), FUNC_STR);
		return;
	}

	if (!WidgetClass->ImplementsInterface(UCinematicPlayerInterface::StaticClass()))
	{
		UE_LOG(LogCinematicPlayerContent, Warning, TEXT("%s :: WidgetClass class must implement CinematicPlayerInterface!"), FUNC_STR);
		return;
	}

	PlayerWidget = CreateWidget(OwningPlayerController.Get(), WidgetClass);
	if (PlayerWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_Initialize(PlayerWidget.Get(), this);
		PlayerWidget->AddToViewport(WidgetsZOrder);
	}
}

void ACinematicPlayerContent::RemovePlayerWidget()
{
	if (PlayerWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_HideAndDestroy(PlayerWidget.Get());
	}

	PlayerWidget.Reset();
}
