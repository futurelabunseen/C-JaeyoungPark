
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
			// 새로운 타이머를 설정하기 전에 기존의 타이머를 중지
			StopTimer(OwnerActor);

			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);

			// 주기적으로 히트를 체크하기 위한 타이머 시작
			bTimerSet = true;
			OwnerActor->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [OwnerActor, this, PayloadData]()
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);
					UE_LOG(LogTemp, Warning, TEXT("Attack hit check triggered: %s"), *TriggerGameplayTag.ToString());

					// 실행 후 타이머 플래그를 재설정
					bTimerSet = false;
				}, 0.2f, false); // 단일 실행 타이머
		}
	}
}