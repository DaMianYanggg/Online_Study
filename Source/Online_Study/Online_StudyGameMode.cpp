// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Online_StudyGameMode.h"
#include "Online_StudyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOnline_StudyGameMode::AOnline_StudyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
