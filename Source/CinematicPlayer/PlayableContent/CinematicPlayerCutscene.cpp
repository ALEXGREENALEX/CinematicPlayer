#include "CinematicPlayerCutscene.h"
#include <Engine/World.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include "Logs/CinematicPlayerLogs.h"

ACinematicPlayerCutscene::ACinematicPlayerCutscene(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  LevelSequence(nullptr),
	  LevelSequenceActorClass(ALevelSequenceActor::StaticClass())
{
}

void ACinematicPlayerCutscene::BeginPlay()
{
	if (!IsValid(LevelSequence))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: LevelSequence is Not Valid!"), FUNC_STR);
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World) || World->bIsTearingDown)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.bAllowDuringConstructionScript = true;
	SpawnParams.bDeferConstruction = true; // Defer construction for autoplay so that BeginPlay() is called

	LevelSequenceActor = World->SpawnActor<ALevelSequenceActor>(LevelSequenceActorClass, SpawnParams);
	LevelSequenceActor->SetSequence(LevelSequence);
	LevelSequenceActor->InitializePlayer();

	const FTransform DefaultTransform;
	LevelSequenceActor->FinishSpawning(DefaultTransform);

	LevelSequencePlayer = LevelSequenceActor->SequencePlayer;
	if (!LevelSequencePlayer.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: LevelSequencePlayer is Not Valid!"), FUNC_STR);
		Destroy();
		return;
	}

	LevelSequencePlayer->OnPlay.AddDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
	LevelSequencePlayer->OnFinished.AddDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);

	Super::BeginPlay();
}

void ACinematicPlayerCutscene::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
		LevelSequencePlayer->OnStop.RemoveDynamic(this, &ACinematicPlayerCutscene::OnStopCallback);
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);

		LevelSequencePlayer->Stop();
	}

	LevelSequencePlayer = nullptr;

	if (LevelSequenceActor.IsValid())
	{
		LevelSequenceActor->Destroy();
		LevelSequenceActor = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerCutscene::OpenAndPlayContent()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->Play();
	}
}

void ACinematicPlayerCutscene::Stop()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);
		LevelSequencePlayer->OnStop.AddDynamic(this, &ACinematicPlayerCutscene::OnStopCallback);
		LevelSequencePlayer->Stop();
	}
}

void ACinematicPlayerCutscene::Pause()
{
	if (LevelSequencePlayer.IsValid() && LevelSequencePlayer->IsPlaying())
	{
		LevelSequencePlayer->Pause();
	}
}

void ACinematicPlayerCutscene::Resume()
{
	if (LevelSequencePlayer.IsValid() && LevelSequencePlayer->IsPaused())
	{
		LevelSequencePlayer->Play();
	}
}

void ACinematicPlayerCutscene::OnPlayCallback()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
	}

	PlaybackStarted();
}

void ACinematicPlayerCutscene::OnStopCallback()
{
	StopAndDestroy();
}

void ACinematicPlayerCutscene::OnFinishedCallback()
{
	FinishAndDestroy();
}
