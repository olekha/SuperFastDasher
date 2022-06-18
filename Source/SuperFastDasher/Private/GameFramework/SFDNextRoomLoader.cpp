// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDNextRoomLoader.h"
#include "GameFramework/SFDGameMode.h"
#include "Player/SFDCharacter.h"
#include "GameFramework/SFDLevelsManager.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ASFDNextRoomLoader::ASFDNextRoomLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxTrigerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigerComponent"));
	RootComponent = BoxTrigerComponent;
	
	NextRoomIndexTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NextRoomIndexTextComponent"));
	NextRoomIndexTextComponent->SetupAttachment(RootComponent);

	PlayerSpawnPointComponent = CreateDefaultSubobject<USceneComponent>("PlayerSpawnPointComponent");
	PlayerSpawnPointComponent->SetupAttachment(RootComponent);
}

void ASFDNextRoomLoader::SetRoomToLoadIndex(const uint8 InRoomIndex)
{
	RoomToLoadIndex = InRoomIndex;

	NextRoomIndexTextComponent->SetText(FText::AsNumber(RoomToLoadIndex));
}

void ASFDNextRoomLoader::SetLocalIndex(const uint8 InLocalIndex)
{
	LocalIndex = InLocalIndex;
}

// Called when the game starts or when spawned
void ASFDNextRoomLoader::BeginPlay()
{
	Super::BeginPlay();

	BoxTrigerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASFDNextRoomLoader::OnBoxComponentBeginOverlapped);
	BoxTrigerComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ASFDNextRoomLoader::OnBoxComponentEndOverlapped);
}

void ASFDNextRoomLoader::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	BoxTrigerComponent->OnComponentBeginOverlap.RemoveAll(this);
}

void ASFDNextRoomLoader::OnBoxComponentBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValid(Other)
		|| !Other->IsA<ASFDCharacter>()
		|| SpawnDisabledTillEndOverlap)
	{
		return;
	}

	ASFDCharacter* Character = CastChecked<ASFDCharacter>(Other);
	Character->OnEnteredIntoRoomLoader();
	
	UE_LOG(LogTemp, Error, TEXT("Try to load level %i"), RoomToLoadIndex);

	USFDLevelsManager* LevelsManager =  SFD::GetLevelsManager(this);
	if(!ensureAlways(IsValid(LevelsManager)))
	{
		return;
	}
		
	LevelsManager->SpawnNextRoom(this);
}

void ASFDNextRoomLoader::OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!IsValid(OtherActor)
		|| !OtherActor->IsA<ASFDCharacter>())
	{
		return;
	}

	ASFDCharacter* Character = CastChecked<ASFDCharacter>(OtherActor);
	Character->OnStepOutFromRoomLoader();
	
	SpawnDisabledTillEndOverlap = false;
}
