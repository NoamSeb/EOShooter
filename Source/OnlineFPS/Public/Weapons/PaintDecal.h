// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintDecal.generated.h"


UCLASS()
class ONLINEFPS_API APaintDecal : public AActor
{
	GENERATED_BODY()
	
	
public:
	// Sets default values for this actor's properties
	APaintDecal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal Settings")
	UDecalComponent* DecalComponent;
	
	UPROPERTY(EditAnywhere, Category = "Decal Settings")
	TArray<UTexture2D*> DecalTextures; 
};
