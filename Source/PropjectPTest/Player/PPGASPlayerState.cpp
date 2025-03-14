// Fill out your copyright notice i the Description page of Project Settings.


#include "Player/PPGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Attribute/PPCharacterSkillAttributeSet.h"


APPGASPlayerState::APPGASPlayerState()
{

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	// 원본에서 복제본으로 계속 전송이 되어야 하므로 속성 설정
	ASC->SetIsReplicated(true);

	// Mixed모드로 설정해야 네트워크 대역폭에서 최고의 성능을 얻을 수 있다.
	// Mixed Mode : GameplayTag 및 GameplayCues가 모든 사람들에게 복제됨
	// + 최소한의 게임플레이 효과 정보만 SimulatedProxy에 복제, 전체 정보는 Owner, AutonomousPorxy에 복제
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPPCharacterAttributeSet>(TEXT("AttributeSet"));
	SkillAttributeSet = CreateDefaultSubobject<UPPCharacterSkillAttributeSet>(TEXT("SkillAttributeSet"));
	NetUpdateFrequency = 50.0f;
}

UAbilitySystemComponent* APPGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}