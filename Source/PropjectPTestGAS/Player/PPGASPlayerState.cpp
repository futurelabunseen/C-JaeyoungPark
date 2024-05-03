// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PPGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Attribute/PPCharacterSkillAttributeSet.h"


APPGASPlayerState::APPGASPlayerState()
{

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	// �������� ���������� ��� ������ �Ǿ�� �ϹǷ� �Ӽ� ����
	// ASC->SetIsReplicated(true);

	// Mixed���� �����ؾ� ��Ʈ��ũ �뿪������ �ְ��� ������ ���� �� �ִ�.
	// Mixed Mode : GameplayTag �� GameplayCues�� ��� ����鿡�� ������
	// + �ּ����� �����÷��� ȿ�� ������ SimulatedProxy�� ����, ��ü ������ Owner, AutonomousPorxy�� ����
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPPCharacterAttributeSet>(TEXT("AttributeSet"));
	SkillAttributeSet = CreateDefaultSubobject<UPPCharacterSkillAttributeSet>(TEXT("SkillAttributeSet"));
	NetUpdateFrequency = 100;
}

UAbilitySystemComponent* APPGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
