// Bouy which floats upon the ocean and gives information such as wind direction and compass

#pragma once

#include "GameFramework/Actor.h"
#include "Buoy.generated.h"

UCLASS()
class CUSTOMMESHTEST_API ABuoy : public AActor
{
	GENERATED_BODY()

private:

	//part rotations

	float CompassRot;

	float VaneRot;

	//foam particles

	float FoamIntervalTimer;

protected:

	//actor components
	UPROPERTY(EditAnywhere, Category = Components)
	USkeletalMeshComponent* BuoyMesh;

	UPROPERTY(EditAnywhere, Category = Components)
	class UFloatComponent* FloatComp;

	//animation
	class UBuoyAnimInstance* BuoyAnim;

public:	
	// Sets default values for this actor's properties
	ABuoy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//part parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float VaneResist;

	//foam parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float FoamInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float FoamSpawnDistance;
	
};
