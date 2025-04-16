#include "ALobbyGameMode.h"

#include "LevelSequencePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "MovieSceneSequencePlayer.h"

ALobbyGameMode::ALobbyGameMode()
{
	// Set default class for the lobby character (BP_Player)

	GameplayLevelName = TEXT("ThirdPersonMap");
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::OnSpaceshipInteraction()
{
	// Play the entry sequence if available
	if (SpaceshipEntrySequence)
	{
		// Create sequence player
		ALevelSequenceActor* LevelSequenceActor;
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.bAutoPlay = true;

		ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(),
			SpaceshipEntrySequence,
			Settings,
			LevelSequenceActor
		);

		if (SequencePlayer)
		{
			// Bind to sequence finish
			SequencePlayer->OnFinished.AddDynamic(this, &ALobbyGameMode::OnEntrySequenceFinished);
			SequencePlayer->Play();
		}
	}
	else
	{
		// If no sequence, transition directly
		OnEntrySequenceFinished();
	}
}

void ALobbyGameMode::OnEntrySequenceFinished()
{
	TransitionToGameplay();
}

void ALobbyGameMode::TransitionToGameplay()
{
	// Save any necessary data before transition
	UGameplayStatics::OpenLevel(this, GameplayLevelName);
}