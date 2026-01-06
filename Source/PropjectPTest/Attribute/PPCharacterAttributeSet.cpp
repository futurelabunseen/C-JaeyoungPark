#include "Attribute/PPCharacterAttributeSet.h"
#include "PropjectPTest.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"
#include "Character/PPGASCharacter.h"

UPPCharacterAttributeSet::UPPCharacterAttributeSet() :
	AttackRange(100.0f),
	MaxAttackRange(300.0f),

	AttackRadius(50.f),
	MaxAttackRadius(150.0f),

	AttackRate(30.0f),
	MaxAttackRate(100.0f),

	Damage(0.0f),
	Health(100.0f),
	MaxHealth(100.0f),

	// 초기화
	Mana(0.0f),
	MaxMana(100.0f),
	Experience(0.0f),
	MaxExperience(100.0f)
{
	InitHealth(GetMaxHealth());
	InitMana(GetMaxMana()); // 마나 초기화
	InitExperience(0.0f);   // 경험치 초기화 (보통 0부터 시작)
}

void UPPCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, MaxExperience, COND_None, REPNOTIFY_Always);
}

void UPPCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

bool UPPCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			if (Data.Target.HasMatchingGameplayTag(PPTAG_CHARACTER_INVINSIBLE))
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}
	return true;
}

void UPPCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumValue = 0.0f; // 공통 최소값

	// --- Health Logic ---
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		//PPGAS_LOG(LogPPGAS, Warning, TEXT("Direct Health Access : %f"), GetHealth());
		SetHealth(FMath::Clamp(GetHealth(), MinimumValue, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		//PPGAS_LOG(LogPPGAS, Log, TEXT("Damage : %f"), GetDamage());
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumValue, GetMaxHealth()));
		SetDamage(0.0f);
	}

	// --- Mana Logic ---
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// 마나는 0 ~ MaxMana 사이로 유지
		SetMana(FMath::Clamp(GetMana(), MinimumValue, GetMaxMana()));
	}

	// --- Experience Logic ---
	if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
	{
		float CurrentExp = GetExperience();
		float MaxExp = GetMaxExperience();

		if (MaxExp > 0.0f && CurrentExp >= MaxExp)
		{
			float Remainder = CurrentExp - MaxExp;
			SetExperience(Remainder);

			// ASC가 붙어있는 아바타(실제 조종 캐릭터)를 가져와야 함
			APPGASCharacter* OwnerCharacter = Cast<APPGASCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());

			if (OwnerCharacter) OwnerCharacter->LevelUp();
		}
	}

	// Dead Check
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		Data.Target.AddLooseGameplayTag(PPTAG_CHARACTER_ISDEAD);
		OnOutOfHealth_Player.Broadcast();
	}
	bOutOfHealth = (GetHealth() <= 0.0f);
}

// -----------------------------------------------

// OnRep Section
void UPPCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	//PPGAS_LOG(LogPPGAS, Warning, TEXT("Health : %f"), GetHealth());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Health, OldHealth);
}

void UPPCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	//PPGAS_LOG(LogPPGAS, Warning, TEXT("MaxHealth : %f"), GetMaxHealth());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UPPCharacterAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Damage, OldDamage);
}

void UPPCharacterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Mana, OldMana);
}

void UPPCharacterAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, MaxMana, OldMaxMana);
}

void UPPCharacterAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldExperience)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Experience, OldExperience);
}

void UPPCharacterAttributeSet::OnRep_MaxExperience(const FGameplayAttributeData& OldMaxExperience)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, MaxExperience, OldMaxExperience);
}