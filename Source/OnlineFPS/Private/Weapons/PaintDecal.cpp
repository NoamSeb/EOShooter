// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PaintDecal.h"
#include "Components/DecalComponent.h"


// Sets default values
APaintDecal::APaintDecal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
}

// Called when the game starts or when spawned
void APaintDecal::BeginPlay()
{
	Super::BeginPlay();
	if (DecalTextures.Num() > 0 && DecalComponent)
	{
		int32 RandomIndex = FMath::RandRange(0, DecalTextures.Num() - 1);
		UTexture2D* SelectedTexture = DecalTextures[RandomIndex];

		if (SelectedTexture)
		{
			
			UMaterialInstanceDynamic* DynMaterial = DecalComponent->CreateDynamicMaterialInstance();
            
			if (DynMaterial)
			{
				DynMaterial->SetTextureParameterValue(FName("DecalTexture"), SelectedTexture);
			}
		}
	}
}

// Called every frame
void APaintDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

