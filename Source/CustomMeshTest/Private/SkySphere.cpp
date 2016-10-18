// Visual only - sphere with cubemap that follows the player to give the appearance of a sky

#include "CustomMeshTest.h"
#include "SkySphere.h"


// Sets default values
ASkySphere::ASkySphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//sky mesh

	SkyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkyMesh"));
	RootComponent = SkyMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkyMeshClassFinder(TEXT("/Game/Sky/skysphere"));
	SkyMesh->SetStaticMesh(SkyMeshClassFinder.Object);
}

// Called when the game starts or when spawned
void ASkySphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkySphere::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

