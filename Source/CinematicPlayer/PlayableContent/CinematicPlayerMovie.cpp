#include "CinematicPlayerMovie.h"
#include <GameFramework/PlayerController.h>
#include <FileMediaSource.h>
#include <MediaPlayer.h>

void ACinematicPlayerMovie::BeginPlay()
{
	if (!IsValid(MediaPlayer))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: MediaPlayer isn't Valid!"), ANSI_TO_TCHAR(__FUNCTION__));
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
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: Can't Play MediaFile!"), ANSI_TO_TCHAR(__FUNCTION__));
		StopAndDestroy();
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
		const bool bNeedPauseOnStart = bCanPause && IsValid(OwningPlayerController) && OwningPlayerController->IsPaused();
		if (!bNeedPauseOnStart)
		{
			MediaPlayer->Play();
			PlaybackStarted();
		}
	}
	else // On some Error
	{
		StopAndDestroy();
	}
}

void ACinematicPlayerMovie::OnMediaOpenFailed(FString FailedUrl)
{
	StopAndDestroy();
}

void ACinematicPlayerMovie::OnMediaClosed()
{
	StopAndDestroy();
}

void ACinematicPlayerMovie::OnEndReached()
{
	FinishAndDestroy();
}
