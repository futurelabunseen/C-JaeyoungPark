
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

	// �����Ƽ �ý��� ������Ʈ ��������
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent is null"));
		return;
	}

	// ��� ���� �±� �߰�
	FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag(FName("Event.Boss.BlockAttack"));
	AbilitySystemComponent->AddLooseGameplayTag(BlockTag);
	UE_LOG(LogTemp, Warning, TEXT("Block tag added: %s"), *BlockTag.ToString());

	// ���� �� ��� ���� �±� ���� (��: ���� ��Ʈ üũ ���Ḧ �ùķ���Ʈ�ϱ� ����)
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
