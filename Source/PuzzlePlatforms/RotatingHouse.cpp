// Fill out your copyright notice in the Description page of Project Settings.

#include "RotatingHouse.h"

// Sets default values
ARotatingHouse::ARotatingHouse()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARotatingHouse::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARotatingHouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();
	FRotator NewRotation = GetActorRotation();
	float RunningTime = GetGameTimeSinceCreation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Z += DeltaHeight * 20.0f;	 // Scale our height by a factor of 20
	float DeltaRotation = DeltaTime * 20.0f; // Rotate by 20 degrees per second
	NewRotation.Yaw += DeltaRotation;
	SetActorRotation(NewRotation);
}
