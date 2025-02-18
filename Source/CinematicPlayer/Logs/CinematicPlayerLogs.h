// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Logging/StructuredLog.h>

#ifndef FUNC_STR
#define FUNC_STR StringCast<TCHAR>(__FUNCTION__).Get()
#endif

CINEMATICPLAYER_API DECLARE_LOG_CATEGORY_EXTERN(LogCinematicPlayer, Log, All);
