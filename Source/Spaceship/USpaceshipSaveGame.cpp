#include "USpaceshipSaveGame.h"

USpaceshipSaveGame::USpaceshipSaveGame()
{
	// Default values
	ProjectileSkinID = 0;
	ColorSkinID = 0;
	Difficulty = 5; // Medium difficulty by default
	VisualizerShape = EVisualizerShape::Circle; // Default visualizer shape
	bFirstTime = true;
	LastScore = 0;
	LastSong = nullptr;
}