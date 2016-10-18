// Game mode for the boat game - controls the game wave manager

#pragma once

#include "GameFramework/GameMode.h"
#include "CustomMeshTestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMESHTEST_API ACustomMeshTestGameMode : public AGameMode
{
	GENERATED_BODY()

private:

	//game wave manager
	UPROPERTY()
	class UWaveManager* WaveManager;

public:


	// Sets default values for this actor's properties
	ACustomMeshTestGameMode(const FObjectInitializer& ObjectInitializer);

	// Create the game wave manager
	UFUNCTION(BlueprintCallable, Category = WaveManagement)
	void CreateWaveManager();

	// Global access point for getting game wave manager
	UFUNCTION(BlueprintCallable, Category = WaveManagement)
	class UWaveManager* GetWaveManager();
	
	// Game wind vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wind)
	FVector Wind;
};
