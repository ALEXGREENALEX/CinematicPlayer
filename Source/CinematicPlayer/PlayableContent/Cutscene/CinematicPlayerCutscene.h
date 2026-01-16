// Copyright 2023 - 2026 Alex Zelenskyi. All Rights Reserved.

#pragma once

#include "PlayableContent/CinematicPlayerContent.h"
#include <Templates/SubclassOf.h>
#include "CinematicPlayerCutscene.generated.h"

class ALevelSequenceActor;
class ULevelSequence;
class ULevelSequencePlayer;

/**
 * Allow to play Cutscenes (Level Sequences).
 */
UCLASS(Abstract, HideDropdown, Blueprintable)
class CINEMATICPLAYER_API ACinematicPlayerCutscene : public ACinematicPlayerContent
{
	GENERATED_BODY()

public:
	ACinematicPlayerCutscene(const FObjectInitializer& ObjectInitializer);

	// Begin ACinematicPlayerContent overrides
	virtual void PostInitializeComponents() override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool OpenAndPlayContent() override;
	virtual void RequestDestroy() override;

#pragma region Data Validation
#if WITH_EDITOR
	// Begin ACinematicPlayerContent overrides
	virtual void ValidateData(FCinematicDataValidationContext& Context) const override;
	// End ACinematicPlayerContent overrides
#endif
#pragma endregion Data Validation

public:
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Resume() override;
	// End ACinematicPlayerContent overrides

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer|Cutscene")
	ALevelSequenceActor* GetLevelSequenceActor() const;

	UFUNCTION(BlueprintPure, Category = "CinematicPlayer|Cutscene")
	ULevelSequencePlayer* GetLevelSequencePlayer() const;

protected:
	UFUNCTION()
	void OnPlay_Callback();

	UFUNCTION()
	void OnStop_Callback();

	UFUNCTION()
	void OnFinished_Callback();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	TObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	TSubclassOf<ALevelSequenceActor> LevelSequenceActorClass;

private:
	TWeakObjectPtr<ALevelSequenceActor> LevelSequenceActor;
};
