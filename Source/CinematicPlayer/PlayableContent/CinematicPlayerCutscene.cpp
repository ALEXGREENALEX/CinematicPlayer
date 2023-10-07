#include "CinematicPlayerCutscene.h"
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>

void ACinematicPlayerCutscene::BeginPlay()
{
	ALevelSequenceActor* SequenceActor;
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, LevelSequence, {}, SequenceActor);
	LevelSequenceActor = SequenceActor;

	if (!LevelSequencePlayer.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: LevelSequencePlayer isn't Valid!"), ANSI_TO_TCHAR(__FUNCTION__));
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
