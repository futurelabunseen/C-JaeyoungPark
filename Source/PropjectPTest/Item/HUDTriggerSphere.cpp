// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/HUDTriggerSphere.h"
#include "Player/PPHUD.h"
#include "Monster/Boss_Mermaid.h"
#include "Kismet/GameplayStatics.h"

AHUDTriggerSphere::AHUDTriggerSphere()
{
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHUDTriggerSphere::OnOverlapBegin);

	// Replication ����
	// bReplicates = true;
}

void AHUDTriggerSphere::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// OtherActor�� �÷��̾� ĳ�������� Ȯ���մϴ�.
		APawn* PlayerPawn = Cast<APawn>(OtherActor);
		if (PlayerPawn)
		{
			APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController());
			if (PlayerController)
			{
				APPHUD* PlayerHUD = Cast<APPHUD>(PlayerController->GetHUD());
				if (PlayerHUD)
				{
					// ������ ã�� ���� �ڵ�
					TArray<AActor*> FoundActors;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss_Mermaid::StaticClass(), FoundActors);
					if (FoundActors.Num() > 0)
					{
						ABoss_Mermaid* Boss = Cast<ABoss_Mermaid>(FoundActors[0]);
						if (Boss)
						{
							PlayerHUD->ShowBossHealthBar(Boss);

							/*GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							this->Destroy();*/
						}
					}
				}
			}
		}
	}
}
