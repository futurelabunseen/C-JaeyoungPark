
#include "AnimNotify_AttackHitCheckEnd.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"

UAnimNotify_AttackHitCheckEnd::UAnimNotify_AttackHitCheckEnd()
{
}

FString UAnimNotify_AttackHitCheckEnd::GetNotifyName_Implementation() const
{
	return TEXT("AttackHitCheckEnd");
}

void UAnimNotify_AttackHitCheckEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComp is null"));
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerActor is null"));
		return;
	}

	// 어빌리티 시스템 컴포넌트 가져오기
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent is null"));
		return;
	}

	// 블록 공격 태그 추가
	FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag(FName("Event.Boss.BlockAttack"));
	AbilitySystemComponent->AddLooseGameplayTag(BlockTag);
	UE_LOG(LogTemp, Warning, TEXT("Block tag added: %s"), *BlockTag.ToString());

	// 지연 후 블록 공격 태그 제거 (예: 공격 히트 체크 종료를 시뮬레이트하기 위해)
	FTimerHandle TimerHandle;
	OwnerActor->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [AbilitySystemComponent, BlockTag]()
		{
			if (IsValid(AbilitySystemComponent))
			{
				AbilitySystemComponent->RemoveLooseGameplayTag(BlockTag);
				UE_LOG(LogTemp, Warning, TEXT("Block tag removed: %s"), *BlockTag.ToString());
			}
		}, 1.0f, false);
}
