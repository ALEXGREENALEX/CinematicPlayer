#pragma once

#include "CinematicPlayerContent.h"
#include "CinematicPlayerMovie.generated.h"

class UFileMediaSource;
class UMediaPlayer;

UCLASS(Abstract, HideDropdown, Blueprintable)
class CINEMATICPLAYER_API ACinematicPlayerMovie : public ACinematicPlayerContent
{
	GENERATED_BODY()

public:
	// Begin ACinematicPlayerContent overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OpenAndPlayContent() override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Resume() override;
	// End ACinematicPlayerContent overrides

private:
	UFUNCTION()
	void OnMediaOpened(FString OpenedUrl);

	UFUNCTION()
	void OnMediaOpenFailed(FString FailedUrl);

	UFUNCTION()
	void OnMediaClosed();

	UFUNCTION()
	void OnEndReached();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	UFileMediaSource* MediaFile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	bool bCanPause = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer", AdvancedDisplay)
	UMediaPlayer* MediaPlayer;
};
