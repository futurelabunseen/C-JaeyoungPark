#include "LevelTransferVolume.h"
#include "Character/PPGASCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ALevelTransferVolume::ALevelTransferVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));
	RootComponent = TransferVolume;
	TransferVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void ALevelTransferVolume::BeginPlay()
{
	Super::BeginPlay();

}

void ALevelTransferVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	APPGASCharacter* Character = Cast<APPGASCharacter>(OtherActor);
	if (Character)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TransferLevelName));
	}
}

