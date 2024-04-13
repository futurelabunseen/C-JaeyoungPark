// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PPGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Player/PPGASPlayerState.h"
#include "EnhancedInputComponent.h"
#include "UI/PPGASWidgetComponent.h"
#include "UI/PPGASUserWidget.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"
#include "PropjectPTestGAS.h"

APPGASCharacter::APPGASCharacter()
{
	ASC = nullptr; // 플레이어 스테이트에서 이미 하나 생성했기 때문에 의도적으로 null로 설정

	static ConstructorHelpers::FObjectFinder<UAnimMontage>ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKillActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_SkillAttack.AM_SkillAttack'"));
	if (SKillActionMontageRef.Object)
	{
		SkillActionMontage = SKillActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DashActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_Dash.AM_Dash'"));
	if (DashActionMontageRef.Object)
	{
		DashActionMontage = DashActionMontageRef.Object;
	}

	HpBar = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

UAbilitySystemComponent* APPGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APPGASPlayerState* GASPS = GetPlayerState<APPGASPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		const UPPCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}

		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}

		for (const auto& StartInputAbility : StartInputAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartInputAbility.Value);
			StartSpec.InputID = StartInputAbility.Key;
			ASC->GiveAbility(StartSpec);
		}

		SetupGASInputComponent(); // PossessedBy 는 서버에서만 호출됨, 클라에서 호출 안 됨

		APlayerController* PlayerContorller = CastChecked<APlayerController>(NewController);
		PlayerContorller->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

void APPGASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupGASInputComponent();
}

void APPGASCharacter::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APPGASCharacter::GASInputReleased, 0);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 1);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 2);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 3);

	}
}

void APPGASCharacter::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
	// NotifyComboActionEnd();
}

void APPGASCharacter::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void APPGASCharacter::OnOutOfHealth()
{
	SetDead();
}

//void APPGASCharacter::NotifyComboActionEnd()
//{
//}
