// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDNextRoomLoader.h"
#include "Player/SFDCharacter.h"
#include "GameFramework/SFDLevelsManager.h"
#include "GameFramework/SFDLevelCore.h"

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

void ASFDNextRoomLoader::SetOwnerRoomIndex(const uint8 InNewOwnerRoomIndex)
{
	OwnerRoomIndex = InNewOwnerRoomIndex;
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
	BoxTrigerComponent->OnComponentEndOverlap.RemoveAll(this);
}

void ASFDNextRoomLoader::OnBoxComponentBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValid(Other)
		|| !Other->IsA<ASFDCharacter>())
	{
		return;
	}
	
	if(OnPlayerStepIntoLoader.IsBound())
	{
		OnPlayerStepIntoLoader.Broadcast(OwnerRoomIndex, LocalIndex);
	}
	
	const ASFDLevelCore* LevelCore = SFD::GetLevelCore(this, OwnerRoomIndex);
	if(!ensureAlways(IsValid(LevelCore)))
	{
		return;
	}

	if(!LevelCore->IsTeleportsEnabled())
	{
		return;
	}
	
	USFDLevelsManager* LevelsManager =  SFD::GetLevelsManager(this);
	if(!ensureAlways(IsValid(LevelsManager)))
	{
		return;
	}
	
	ASFDCharacter* Character = CastChecked<ASFDCharacter>(Other);	
	Character->OnPreTeleportToTheNextRoom();

	LevelsManager->RequestTransitionToTheNextRoom(RoomToLoadIndex, LocalIndex);
}

void ASFDNextRoomLoader::OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!IsValid(OtherActor)
		|| !OtherActor->IsA<ASFDCharacter>())
	{
		return;
	}

	if(OnPlayerStepOutOfLoader.IsBound())
	{
		OnPlayerStepOutOfLoader.Broadcast(OwnerRoomIndex, LocalIndex);
	}
}
