// Component simulating the physics of an object in the ocean, floating on the waves

#pragma once

#include "Components/ActorComponent.h"
#include "FloatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMMESHTEST_API UFloatComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	//location/velocities of the float

	FVector PositionOnOcean;

	FVector Velocity;

	FRotator AVelocity;

	bool IsInAir;

	float CurrentCombinedDrag;

	//game wave manager
	class UWaveManager* WaveManager;

public:	
	// Sets default values for this component's properties
	UFloatComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	//motion parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Drag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SideDrag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Gravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Bouyancy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BouyancyDrag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BouyancyAngular;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BouyancyAngularResist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Slide;
	
	// Input angular yaw velocity
	void SetYawVelocity(float Val);

	float GetYawVelocity();

	// Estimate moving average of drag affecting the float
	float GetCurrentDrag();

	// Input velocity
	void SetVelocity(FVector Val);

	FVector GetVelocity();

	bool GetIsInAir();
};
