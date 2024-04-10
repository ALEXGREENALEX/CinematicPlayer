#include "CinematicPlayerContent.h"
#include <Blueprint/UserWidget.h>
#include <EnhancedInputSubsystems.h>
#include <InputMappingContext.h>
#include "Logs/CinematicPlayerLogs.h"
#include "Interfaces/CinematicPlayerInterface.h"

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

	EnableInput(OwningPlayerController.Get()); // Enable receive Input from PlayerController
	AddInputMapping();

	PlayerUserWidget = CreatePlayerWidget(OwningPlayerController.Get());

	OpenAndPlayContent();
}

void ACinematicPlayerContent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveInputMapping();

	if (OwningPlayerController.IsValid())
	{
		DisableInput(OwningPlayerController.Get()); // Disable receive Input from PlayerController
	}

	RemovePlayerWidget(PlayerUserWidget.Get());
	PlayerUserWidget.Reset();

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerContent::PlaybackStarted()
{
	ReceiveOnStart();

	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
	}

	if (PlayerUserWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_Start(PlayerUserWidget.Get());
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
	if (PlayerUserWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_AnyKeyPressed(PlayerUserWidget.Get(), bPressed);
	}
}

void ACinematicPlayerContent::PressSkipKey(bool bPressed)
{
	if (PlayerUserWidget.IsValid())
	{
		ICinematicPlayerInterface::Execute_SkipKeyPressed(PlayerUserWidget.Get(), bPressed);
	}
}

void ACinematicPlayerContent::AddInputMapping_Implementation()
{
	if (!IsValid(InputMappingContext))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: InputMappingContext is Not Valid!"), FUNC_STR);
		return;
	}

	if (!OwningPlayerController.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: OwningPlayerController is Not Valid!"), FUNC_STR);
		return;
	}

	const ULocalPlayer* LocalPlayer = OwningPlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: LocalPlayer is Not Valid!"), FUNC_STR);
		return;
	}

	EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!EnhancedInputSubsystem.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: EnhancedInputSubsystem is Not Valid!"), FUNC_STR);
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
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: PlayerController is Not Valid!"), FUNC_STR);
		return nullptr;
	}

	if (!IsValid(WidgetClass))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: WidgetClass is Not Valid!"), FUNC_STR);
		return nullptr;
	}

	if (!WidgetClass->ImplementsInterface(UCinematicPlayerInterface::StaticClass()))
	{
		UE_LOG(LogCinematicPlayerContent, Warning, TEXT("%s :: WidgetClass class must implement CinematicPlayerInterface!"), FUNC_STR);
		return nullptr;
	}

	UUserWidget* PlayerWidget = CreateWidget(OwningPlayerController.Get(), WidgetClass);
	if (!IsValid(PlayerWidget))
	{
		UE_LOG(LogCinematicPlayerContent, Warning, TEXT("%s :: Can't create PlayerWidget!"), FUNC_STR);
		return nullptr;
	}

	ICinematicPlayerInterface::Execute_Initialize(PlayerWidget, this);
	PlayerWidget->AddToViewport(WidgetsZOrder);
	return PlayerWidget;
}

void ACinematicPlayerContent::RemovePlayerWidget_Implementation(UUserWidget* PlayerWidget)
{
	if (IsValid(PlayerWidget))
	{
		ICinematicPlayerInterface::Execute_HideAndDestroy(PlayerWidget);
	}
}
