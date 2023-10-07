#pragma once

#include <UObject/Interface.h>
#include "CinematicPlayerInterface.generated.h"

class ACinematicPlayerContent;

UINTERFACE(NotBlueprintable, MinimalAPI)
class UCinematicPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class CINEMATICPLAYER_API ICinematicPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void Initialize(ACinematicPlayerContent* PlayableContent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void Start();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void HideAndDestroy();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void SkipKeyPressed(bool bPressed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CinematicPlayer")
	void AnyKeyPressed(bool bPressed);
};
