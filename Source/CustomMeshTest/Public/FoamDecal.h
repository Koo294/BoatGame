// Visual only - Foam decal created by boat or other object floating on the ocean's surface to represent small waves created (e.g bow waves)

#pragma once

#include "OceanDecal.h"
#include "FoamDecal.generated.h"

UCLASS()
class CUSTOMMESHTEST_API AFoamDecal : public AOceanDecal
{
	GENERATED_BODY()

private:

	//lifetime

	float Life;

	float CurrentGrowth;

	//dynamic materials

	class UMaterialInstanceDynamic* DynamicFoamMat;
	
public:	
	// Sets default values for this actor's properties
	AFoamDecal();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//lifetime parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life)
	UCurveFloat*  AlphaLifeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life)
	float RotateRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life)
	float Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life)
	float GrowRate;
};
