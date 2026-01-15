// Copyright 2023 - 2026 Alex Zelenskyi. All Rights Reserved.

#pragma once

#include <UObject/Interface.h>
#include "CinematicPlayerWidgetInterface.generated.h"

class ACinematicPlayerContent;

/**
 * CinematicPlayer Widget Interface.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UCinematicPlayerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class CINEMATICPLAYER_API ICinematicPlayerWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void Initialize(bool bCanSkip, ACinematicPlayerContent* PlayableContent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void Start();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void HideAndDestroy();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void SkipKeyPressed(bool bPressed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void AnyKeyPressed(bool bPressed);
};
