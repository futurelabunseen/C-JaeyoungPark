
#include "GA/PPGA_AttackHitCheck.h"
#include "PropjectPTest.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/PPAT_Trace.h"
#include "GA/TA/PPTA_Trace.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"

// ���� ��Ʈ üũ Ŭ���� ������
UPPGA_AttackHitCheck::UPPGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; // �� ���ͺ��� �ν��Ͻ� ���� ��å ����
}

// �����Ƽ Ȱ��ȭ �Լ�
void UPPGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	UPPAT_Trace* AttackTraceTask = UPPAT_Trace::CreateTask(this, TargetActorClass); // ���� ���� �۾� ����
	if (IsValid(AttackTraceTask))
	{
		AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_AttackHitCheck::OnTraceResultCallback); // ���� ��� �ݹ� �Լ� ���
		AttackTraceTask->ReadyForActivation(); // ���� �۾� Ȱ��ȭ �غ�
	}
}

// ���� ��� �ݹ� �Լ�
void UPPGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bool bHitDetected = false; // ��Ʈ ���� ����

	// Ÿ�� ������ ó�� ����
	for (int32 i = 0; i < TargetDataHandle.Data.Num(); ++i)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i)) // Ÿ�� �����Ϳ� ��Ʈ ����� �ִ��� Ȯ��
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i); // ��Ʈ ��� ��������
			PPGAS_LOG(LogPPGAS, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName())); // �α� ���: ������ Ÿ�� �̸�

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor()); // Ÿ���� �����Ƽ �ý��� ������Ʈ ��������

			if (IsValid(TargetASC))
			{
				bHitDetected = true; // ��Ʈ ���� ����
				ApplyEffectsToTarget(TargetASC, TargetDataHandle, i, HitResult); // Ÿ�ٿ� ȿ�� ����
			}
		}
		else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i)) // Ÿ�� �����Ϳ� ���Ͱ� �ִ��� Ȯ��
		{
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked(); // �ҽ� �����Ƽ �ý��� ������Ʈ ��������
			if (IsValid(SourceASC))
			{
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel); // ���� ������ ȿ�� ��� ����
				if (EffectSpecHandle.IsValid())
				{
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle); // Ÿ�ٿ� ȿ�� ����

					FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle); // ȿ�� ���ؽ�Ʈ �ڵ� ��������
					CueContextHandle.AddActors(TargetDataHandle.Data[i].Get()->GetActors(), false); // Ÿ�� �������� ���� �߰�
					FGameplayCueParameters CueParam; // �����÷��� ť �Ķ����
					CueParam.EffectContext = CueContextHandle; // ť �Ķ���Ϳ� ȿ�� ���ؽ�Ʈ �ڵ� ����

					SourceASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam); // ���� ��Ʈ ť ����
				}
			}
		}
	}

	if (!bHitDetected) // ��Ʈ�� �������� ���� ���
	{
		UE_LOG(LogTemp, Log, TEXT("No valid ASC found in target data!")); // �α� ���: ��ȿ�� ASC�� ã�� �� ����
	}

	bool bReplicatedEndAbility = true; // �����Ƽ ���� ���� ����
	bool bWasCancelled = false; // ��� ����
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled); // �����Ƽ ����
}

// Ÿ�ٿ� ȿ�� ���� �Լ�
void UPPGA_AttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index, const FHitResult& HitResult)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked(); // �ҽ� �����Ƽ �ý��� ������Ʈ ��������
	if (!IsValid(SourceASC) || !IsValid(TargetASC)) // �ҽ� �Ǵ� Ÿ�� ASC�� ���� ���
	{
		UE_LOG(LogTemp, Log, TEXT("ASC Not Found!")); // �α� ���: ASC�� ã�� �� ����
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel); // ���� ������ ȿ�� ��� ����
	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle); // Ÿ�ٿ� ȿ�� ����

		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle); // ȿ�� ���ؽ�Ʈ �ڵ� ��������
		CueContextHandle.AddHitResult(HitResult); // ��Ʈ ��� �߰�
		FGameplayCueParameters CueParam; // �����÷��� ť �Ķ����
		CueParam.EffectContext = CueContextHandle; // ť �Ķ���Ϳ� ȿ�� ���ؽ�Ʈ �ڵ� ����

		TargetASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam); // ���� ��Ʈ ť ����
	}

	FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect); // ���� ���� ȿ�� ��� ����
	if (BuffEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle); // �����ڿ��� ȿ�� ����
	}
}