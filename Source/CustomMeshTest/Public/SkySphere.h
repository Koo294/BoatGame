// Visual only - sphere with cubemap that follows the player to give the appearance of a sky

#pragma once

#include "GameFramework/Actor.h"
#include "SkySphere.generated.h"

UCLASS()
class CUSTOMMESHTEST_API ASkySphere : public AActor
{
	GENERATED_BODY()

protected:

	//actor components

	UPROPERTY(EditAnywhere, Category = Components)
	UStaticMeshComponent* SkyMesh;

public:	
	// Sets default values for this actor's properties
	ASkySphere();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
