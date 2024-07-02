
#include "AnimNotify_AttackHitCheckStart.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"

bool UAnimNotify_AttackHitCheckStart::bTimerSet = false; // Initialize the static variable

UAnimNotify_AttackHitCheckStart::UAnimNotify_AttackHitCheckStart()
{
}

FString UAnimNotify_AttackHitCheckStart::GetNotifyName_Implementation() const
{
	return TEXT("AttackHitCheckStart");
}

void UAnimNotify_AttackHitCheckStart::StopTimer(AActor* OwnerActor)
{
	if (OwnerActor && bTimerSet)
	{
		OwnerActor->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		bTimerSet = false;
	}
}

void UAnimNotify_AttackHitCheckStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (IsValid(OwnerActor))
		{
			// ���ο� Ÿ�̸Ӹ� �����ϱ� ���� ������ Ÿ�̸Ӹ� ����
			StopTimer(OwnerActor);

			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);

			// �ֱ������� ��Ʈ�� üũ�ϱ� ���� Ÿ�̸� ����
			bTimerSet = true;
			OwnerActor->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [OwnerActor, this, PayloadData]()
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);
					UE_LOG(LogTemp, Warning, TEXT("Attack hit check triggered: %s"), *TriggerGameplayTag.ToString());

					// ���� �� Ÿ�̸� �÷��׸� �缳��
					bTimerSet = false;
				}, 0.2f, false); // ���� ���� Ÿ�̸�
		}
	}
}