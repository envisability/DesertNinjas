// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DesertNinjasGameMode.h"
#include "DesertNinjasCharacter.h"

ADesertNinjasGameMode::ADesertNinjasGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ADesertNinjasCharacter::StaticClass();	
}
