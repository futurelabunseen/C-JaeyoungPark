#include "GA/PPGA_BossAttackHitCheck.h"
#include "PropjectPTest.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/PPAT_BossAttackTrace.h"
#include "GA/TA/PPTA_BossAttackTrace.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"

UPPGA_BossAttackHitCheck::UPPGA_BossAttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// 능력 활성화
void UPPGA_BossAttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	UPPAT_BossAttackTrace* AttackTraceTask = UPPAT_BossAttackTrace::CreateTask(this, TargetActorClass);  // 보스 공격 추적 작업을 생성하고 초기화
	AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_BossAttackHitCheck::OnTraceResultCallback); // 작업 완료시 호출될 콜백 함수 연결
	AttackTraceTask->ReadyForActivation(); // 작업을 활성화 준비 상태로 설정
}

// 공격 추적 결과를 처리하는 콜백 함수
void UPPGA_BossAttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bool bHitDetected = false;

	for (int32 i = 0; i < TargetDataHandle.Data.Num(); ++i) // 대상 데이터 순회
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i)) // 히트 결과가 있는지 확인
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i); // 히트 결과를 가져옴
			PPGAS_LOG(LogPPGAS, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName()));

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor()); // 대상 ASC 가져오기

			if (TargetASC)
			{
				bHitDetected = true;
				ApplyEffectsToTarget(TargetASC, TargetDataHandle, i, HitResult); // 효과 적용
			}
		}
		else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i)) // 대상 데이터에 액터 확인
		{
			TArray<TWeakObjectPtr<AActor>> TargetActors = TargetDataHandle.Data[i]->GetActors();
			for (TWeakObjectPtr<AActor> TargetActor : TargetActors)
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor.Get()); // 대상 ASC 가져오기
				if (TargetASC)
				{
					bHitDetected = true;
					ApplyEffectsToTarget(TargetASC, TargetDataHandle, i);
				}
			}
		}
	}
	if (!bHitDetected)
	{
		UE_LOG(LogTemp, Log, TEXT("No valid ASC found in target data!"));
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled); // 능력 종료 및 상태 업데이트
}

void UPPGA_BossAttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index, const FHitResult& HitResult)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	if (!SourceASC || !TargetASC)
	{
		UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		CueContextHandle.AddHitResult(HitResult);
		FGameplayCueParameters CueParam;
		CueParam.EffectContext = CueContextHandle;

		// TargetASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam);
	}

	FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
	if (BuffEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
	}
}

void UPPGA_BossAttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	if (!SourceASC || !TargetASC)
	{
		UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		CueContextHandle.AddActors(TargetDataHandle.Data[Index]->GetActors(), false);
		FGameplayCueParameters CueParam;
		CueParam.EffectContext = CueContextHandle;
	}

	FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
	if (BuffEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
	}
}
