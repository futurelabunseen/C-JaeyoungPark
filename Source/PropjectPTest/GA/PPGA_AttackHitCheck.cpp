
#include "GA/PPGA_AttackHitCheck.h"
#include "PropjectPTest.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/PPAT_Trace.h"
#include "GA/TA/PPTA_Trace.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"

// 공격 히트 체크 클래스 생성자
UPPGA_AttackHitCheck::UPPGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; // 각 액터별로 인스턴스 생성 정책 설정
}

// 어빌리티 활성화 함수
void UPPGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	UPPAT_Trace* AttackTraceTask = UPPAT_Trace::CreateTask(this, TargetActorClass); // 공격 추적 작업 생성
	if (IsValid(AttackTraceTask))
	{
		AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_AttackHitCheck::OnTraceResultCallback); // 추적 결과 콜백 함수 등록
		AttackTraceTask->ReadyForActivation(); // 추적 작업 활성화 준비
	}
}

// 추적 결과 콜백 함수
void UPPGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bool bHitDetected = false; // 히트 감지 여부

	// 타겟 데이터 처리 루프
	for (int32 i = 0; i < TargetDataHandle.Data.Num(); ++i)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i)) // 타겟 데이터에 히트 결과가 있는지 확인
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i); // 히트 결과 가져오기
			PPGAS_LOG(LogPPGAS, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName())); // 로그 출력: 감지된 타겟 이름

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor()); // 타겟의 어빌리티 시스템 컴포넌트 가져오기

			if (IsValid(TargetASC))
			{
				bHitDetected = true; // 히트 감지 설정
				ApplyEffectsToTarget(TargetASC, TargetDataHandle, i, HitResult); // 타겟에 효과 적용
			}
		}
		else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i)) // 타겟 데이터에 액터가 있는지 확인
		{
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked(); // 소스 어빌리티 시스템 컴포넌트 가져오기
			if (IsValid(SourceASC))
			{
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel); // 공격 데미지 효과 사양 생성
				if (EffectSpecHandle.IsValid())
				{
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle); // 타겟에 효과 적용

					FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle); // 효과 컨텍스트 핸들 가져오기
					CueContextHandle.AddActors(TargetDataHandle.Data[i].Get()->GetActors(), false); // 타겟 데이터의 액터 추가
					FGameplayCueParameters CueParam; // 게임플레이 큐 파라미터
					CueParam.EffectContext = CueContextHandle; // 큐 파라미터에 효과 컨텍스트 핸들 설정

					SourceASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam); // 공격 히트 큐 실행
				}
			}
		}
	}

	if (!bHitDetected) // 히트가 감지되지 않은 경우
	{
		UE_LOG(LogTemp, Log, TEXT("No valid ASC found in target data!")); // 로그 출력: 유효한 ASC를 찾을 수 없음
	}

	bool bReplicatedEndAbility = true; // 어빌리티 종료 복제 여부
	bool bWasCancelled = false; // 취소 여부
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled); // 어빌리티 종료
}

// 타겟에 효과 적용 함수
void UPPGA_AttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index, const FHitResult& HitResult)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked(); // 소스 어빌리티 시스템 컴포넌트 가져오기
	if (!IsValid(SourceASC) || !IsValid(TargetASC)) // 소스 또는 타겟 ASC가 없는 경우
	{
		UE_LOG(LogTemp, Log, TEXT("ASC Not Found!")); // 로그 출력: ASC를 찾을 수 없음
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel); // 공격 데미지 효과 사양 생성
	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle); // 타겟에 효과 적용

		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle); // 효과 컨텍스트 핸들 가져오기
		CueContextHandle.AddHitResult(HitResult); // 히트 결과 추가
		FGameplayCueParameters CueParam; // 게임플레이 큐 파라미터
		CueParam.EffectContext = CueContextHandle; // 큐 파라미터에 효과 컨텍스트 핸들 설정

		TargetASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam); // 공격 히트 큐 실행
	}

	FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect); // 공격 버프 효과 사양 생성
	if (BuffEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle); // 소유자에게 효과 적용
	}
}