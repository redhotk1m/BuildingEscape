// Copyright Kim Thorsen
#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Volume.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	
	
	InitialYaw = CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	OpenAngle += InitialYaw;

	if (!Pressureplate)
	{
		UE_LOG(LogTemp, Error, TEXT("(ID: %s), has the OpenDoor component, but no PressurePlate assigned! %s"), *GetOwner()->GetActorLabel(),*GetOwner()->GetName(),Pressureplate);

		//UE_LOG(LogTemp, Error, TEXT("%s (ID: %s), has the OpenDoor component, but no PressurePlate assigned! %s"), *GetOwner()->GetActorLabel(),*GetOwner()->GetName(),Pressureplate);
	}

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (Pressureplate && Pressureplate->IsOverlappingActor(ActorThatOpens)) //Lazy Error handling, if pressureplate != null &&
	if (TotalMassOfActors() > MassRequiredToOpenDoor)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();//When the door was opened
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}


}

void UOpenDoor::OpenDoor(float DeltaTime){
	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpenSpeed);
	FRotator CurrentActorRotation(0.0f, CurrentYaw, 0.0f);
	GetOwner()->SetActorRotation(CurrentActorRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime){
	CurrentYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime, DoorCloseSpeed);
	FRotator CurrentActorRotation(0.0f, CurrentYaw, 0.0f);
	GetOwner()->SetActorRotation(CurrentActorRotation);
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.0f;
	TArray<AActor*> OverlappingActors = nullptr;
	Pressureplate-> GetOverlappingActors(OUT OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

