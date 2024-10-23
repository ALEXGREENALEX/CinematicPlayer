#include "CinematicPlayerMovie.h"
#include <GameFramework/PlayerController.h>
#include <FileMediaSource.h>
#include <MediaPlayer.h>
#include "Logs/CinematicPlayerLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerMovie)

void ACinematicPlayerMovie::BeginPlay()
{
	if (!IsValid(MediaPlayer))
	{
		UE_LOGFMT(LogCinematicPlayerContent, Error, "[{FUNC}] MediaPlayer is Not Valid!", FUNC_STR);
		Destroy();
		return;
	}

	MediaPlayer->PlayOnOpen = false;
	MediaPlayer->SetLooping(false);

	MediaPlayer->OnMediaOpened.AddDynamic(this, &ACinematicPlayerMovie::OnMediaOpened);
	MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &ACinematicPlayerMovie::OnMediaOpenFailed);
	MediaPlayer->OnMediaClosed.AddDynamic(this, &ACinematicPlayerMovie::OnMediaClosed);
	MediaPlayer->OnEndReached.AddDynamic(this, &ACinematicPlayerMovie::OnEndReached);

	Super::BeginPlay();
}

void ACinematicPlayerMovie::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(MediaPlayer))
	{
		MediaPlayer->OnMediaOpened.RemoveDynamic(this, &ACinematicPlayerMovie::OnMediaOpened);
		MediaPlayer->OnMediaOpenFailed.RemoveDynamic(this, &ACinematicPlayerMovie::OnMediaOpenFailed);
		MediaPlayer->OnMediaClosed.RemoveDynamic(this, &ACinematicPlayerMovie::OnMediaClosed);
		MediaPlayer->OnEndReached.RemoveDynamic(this, &ACinematicPlayerMovie::OnEndReached);

		MediaPlayer->Close();
	}

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerMovie::OpenAndPlayContent()
{
	if (!IsValid(MediaPlayer) || !IsValid(MediaFile) || !MediaPlayer->OpenSource(MediaFile))
	{
		UE_LOGFMT(LogCinematicPlayerContent, Error, "[{FUNC}] Can't Play MediaFile!", FUNC_STR);
		PlaybackStopped();
		Destroy(false, false);
	}
}

void ACinematicPlayerMovie::Stop()
{
	if (IsValid(MediaPlayer))
	{
		MediaPlayer->Close();
	}
}

void ACinematicPlayerMovie::Pause()
{
	if (bCanPause && IsValid(MediaPlayer) && MediaPlayer->CanPause() && MediaPlayer->IsPlaying())
	{
		MediaPlayer->Pause();
	}
}

void ACinematicPlayerMovie::Resume()
{
	if (bCanPause && IsValid(MediaPlayer) && !MediaPlayer->IsPlaying())
	{
		MediaPlayer->Play(); // Start or Resume Playback

		if (!MediaPlayer->IsPaused()) // Game was paused on Start
		{
			PlaybackStarted();
		}
	}
}

void ACinematicPlayerMovie::OnMediaOpened(FString OpenedUrl)
{
	if (IsValid(MediaPlayer))
	{
		const bool bNeedPauseOnStart = bCanPause && OwningPlayerController.IsValid() && OwningPlayerController->IsPaused();
		if (!bNeedPauseOnStart)
		{
			MediaPlayer->Play();
			PlaybackStarted();
		}
	}
	else // On some Error
	{
		PlaybackStopped();
		Destroy(false, false);
	}
}

void ACinematicPlayerMovie::OnMediaOpenFailed(FString FailedUrl)
{
	PlaybackStopped();
	Destroy(false, false);
}

void ACinematicPlayerMovie::OnMediaClosed()
{
	PlaybackStopped();
	Destroy(false, false);
}

void ACinematicPlayerMovie::OnEndReached()
{
	PlaybackFinished();
	Destroy(false, false);
}
