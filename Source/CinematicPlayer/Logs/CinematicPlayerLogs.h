// Copyright 2024 Alex Zelenskyi. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>

#define FUNC_STR StringCast<TCHAR>(__FUNCTION__).Get()

CINEMATICPLAYER_API DECLARE_LOG_CATEGORY_EXTERN(LogCinematicPlayer, Log, All);
CINEMATICPLAYER_API DECLARE_LOG_CATEGORY_EXTERN(LogCinematicPlayerContent, Log, All);
