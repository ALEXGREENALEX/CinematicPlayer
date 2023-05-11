#include "PlayCinematicAsync.h"
#include <Engine/World.h>
#include "PlayableContent/CinematicPlayerContent.h"

DEFINE_LOG_CATEGORY(LogCinematicPlayer);

UPlayCinematicAsync* UPlayCinematicAsync::PlayCinematicAsync(UObject* WorldContextObject, APlayerController* PlayerController, TSoftClassPtr<ACinematicPlayerContent> Content)
{
	UPlayCinematicAsync* Action = NewObject<UPlayCinematicAsync>(WorldContextObject);
	Action->PlayerController = PlayerController;
	Action->ContentSoftClass = Content;

	if (PlayerController == nullptr)
	{
		const UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
		if (IsValid(World))
		{
			Action->PlayerController = World->GetFirstPlayerController();
		}
	}

	return Action;
}

void UPlayCinematicAsync::Activate()
{
	Super::Activate();

	if (!PlayerController.IsValid())
	{
		UE_LOG(LogCinematicPlayer, Error, TEXT("%s :: PlayerController isn't Valid!"), ANSI_TO_TCHAR(__FUNCTION__));
		SetReadyToDestroy();
		return;
	}

	const TSubclassOf<ACinematicPlayerContent> ContentClass = ContentSoftClass.IsNull() ? nullptr : ContentSoftClass.LoadSynchronous();
	if (!IsValid(ContentClass))
	{
		UE_LOG(LogCinematicPlayer, Error, TEXT("%s :: ContentClass isn't Valid!"), ANSI_TO_TCHAR(__FUNCTION__));
		SetReadyToDestroy();
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		PlayableContent = World->SpawnActorDeferred<ACinematicPlayerContent>(ContentClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		PlayableContent->OnStart.AddDynamic(this, &UPlayCinematicAsync::StartCallback);
		PlayableContent->OnSkip.AddDynamic(this, &UPlayCinematicAsync::SkipCallback);
		PlayableContent->OnFinish.AddDynamic(this, &UPlayCinematicAsync::FinishCallback);

		PlayableContent->Initialize(PlayerController.Get());
		PlayableContent->FinishSpawning(FTransform::Identity, true);
	}
}

void UPlayCinematicAsync::SetReadyToDestroy()
{
	if (IsValid(PlayableContent))
	{
		PlayableContent->OnStart.RemoveDynamic(this, &UPlayCinematicAsync::StartCallback);
		PlayableContent->OnSkip.RemoveDynamic(this, &UPlayCinematicAsync::SkipCallback);
		PlayableContent->OnFinish.RemoveDynamic(this, &UPlayCinematicAsync::FinishCallback);

		PlayableContent->Destroy();
		PlayableContent = nullptr;
	}

	Super::SetReadyToDestroy();
}

void UPlayCinematicAsync::StartCallback()
{
	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
	}
}

void UPlayCinematicAsync::SkipCallback()
{
	if (OnSkip.IsBound())
	{
		OnSkip.Broadcast();
	}

	SetReadyToDestroy();
}

void UPlayCinematicAsync::FinishCallback()
{
	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast();
	}

	SetReadyToDestroy();
}
