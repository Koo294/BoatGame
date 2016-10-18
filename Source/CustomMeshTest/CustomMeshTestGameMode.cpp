// Game mode for the boat game - controls the game wave manager

#include "CustomMeshTest.h"
#include "WaveManager.h"
#include "GerstnerWaveForm.h"
#include "Boat.h"
#include "BoatController.h"
#include "CustomMeshTestGameMode.h"

// Sets default values for this actor's properties
ACustomMeshTestGameMode::ACustomMeshTestGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CreateWaveManager();

	// set default pawn class to the boat blueprint
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Boat/TestBoatBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ABoatController::StaticClass();
}

// Create the game wave manager
void ACustomMeshTestGameMode::CreateWaveManager()
{
	WaveManager = NewObject<UWaveManager>();
}

// Global access point for getting game wave manager
UWaveManager* ACustomMeshTestGameMode::GetWaveManager()
{
	return WaveManager;
}

