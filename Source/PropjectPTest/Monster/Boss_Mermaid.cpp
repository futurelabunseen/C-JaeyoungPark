#include "Monster/Boss_Mermaid.h"
#include "AI/Boss/BossAIController.h"
#include "Attribute/BossAttributeSet.h"
#include "Player/PPHUD.h"
#include "UI/PPGASWidgetComponent.h"
#include "TimerManager.h"
#include "AbilitySystemBlueprintLibrary.h" 
#include "GameplayTagContainer.h"

ABoss_Mermaid::ABoss_Mermaid()
{
	PrimaryActorTick.bCanEverTick = true;

	// Boss AI Setting
	AIControllerClass = ABossAIController::StaticClass();

	BossAttributeSet = CreateDefaultSubobject<UBossAttributeSet>(TEXT("BossAttributeSet"));

	DamageText = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("DamageTextWidget"));
	DamageText->SetupAttachment(GetMesh());
	static ConstructorHelpers::FClassFinder<UUserWidget> DamageTextWidgetRef(TEXT("/Game/UI/WBP_DamageText.WBP_DamageText_C"));
	if (DamageTextWidgetRef.Class)
	{
		DamageText->SetWidgetClass(DamageTextWidgetRef.Class);
		DamageText->SetWidgetSpace(EWidgetSpace::Screen);
		DamageText->SetDrawSize(FVector2D(200.0f, 50.0f));
		DamageText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABoss_Mermaid::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BossAttributeSet->OnOutOfHealth_Boss.AddDynamic(this, &ABoss_Mermaid::OnOutOfHealth);
}

void ABoss_Mermaid::OnOutOfHealth(AActor* Killer)
{
	// 부모의 사망 로직 호출 (충돌 해제 등 기본 처리)
	Super::OnOutOfHealth(Killer);

	// 서버에서 실행되는지 확인
	if (!HasAuthority())
	{
		return;
	}

	// 킬러에게 경험치 보상 지급 로직 (Golem과 동일)
	if (Killer && ExpRewardEffectClass)
	{
		UAbilitySystemComponent* KillerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Killer);
		if (KillerASC)
		{
			FGameplayEffectContextHandle EffectContext = KillerASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle EffectSpecHandle = KillerASC->MakeOutgoingSpec(ExpRewardEffectClass, 1.0f, EffectContext);

			if (EffectSpecHandle.IsValid())
			{
				// SetByCaller로 경험치 양 전달 (태그: Data.Reward.Exp)
				EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Reward.Exp")), ExpRewardAmount);

				// 적용
				KillerASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);

				//UE_LOG(LogTemp, Warning, TEXT("Boss Reward: Gave %f Exp to %s"), ExpRewardAmount, *Killer->GetName());
			}
		}
	}

	// 기존 보스 사망 처리 (HUD 숨기기, 서버 연결 종료 타이머 등)
	MulticastHidePlayerHUDsRPC();

	FTimerHandle DeadMonsterTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadMonsterTimerHandle, this, &ABoss_Mermaid::DisconnectFromServer, 5.0f, false);
	//UE_LOG(LogTemp, Warning, TEXT("OnOutOfHealth: Timer set for DisconnectFromServer"));
}


void ABoss_Mermaid::DisconnectFromServer()
{
	//UE_LOG(LogTemp, Warning, TEXT("DisconnectFromServer: Called"));

	UWorld* World = GetWorld();
	if (World)
	{

		// 모든 플레이어 컨트롤러를 순회하면서 레벨을 변경합니다.
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (IsValid(PC))
			{
				// 새로운 맵의 이름을 설정합니다.
				FName NewLevelName = TEXT("Demonstration_Village"); // 원하는 맵 이름으로 변경

				// 모든 클라이언트를 새로운 맵으로 이동시킵니다.
				PC->ClientTravel(NewLevelName.ToString(), TRAVEL_Absolute);
				//UE_LOG(LogTemp, Warning, TEXT("DisconnectFromServer: ClientTravel called for PC %s"), *PC->GetName());
			}
			/*else
			{
				UE_LOG(LogTemp, Error, TEXT("DisconnectFromServer: Invalid PlayerController found"));
			}*/
		}
	}
	/*else
	{
		UE_LOG(LogTemp, Error, TEXT("DisconnectFromServer: World is not valid"));
	}*/
}

void ABoss_Mermaid::MulticastHidePlayerHUDsRPC_Implementation()
{
	if (!HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("MulticastHidePlayerHUDsRPC called on a non-server instance"));
		return;
	}

	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (!IsValid(PC))
			{
				//UE_LOG(LogTemp, Error, TEXT("MulticastHidePlayerHUDsRPC: Invalid PlayerController found"));
				continue;
			}

			// HUD가 제대로 할당되어 있는지 확인
			AHUD* RawHUD = PC->GetHUD();
			if (!IsValid(RawHUD))
			{
				//UE_LOG(LogTemp, Error, TEXT("MulticastHidePlayerHUDsRPC: PC %s has no HUD assigned"), *PC->GetName());
				continue;
			}

			// APPHUD로 캐스팅 시도
			APPHUD* PlayerHUD = Cast<APPHUD>(PC->GetHUD());
			/*if (!IsValid(PlayerHUD))
			{
				UE_LOG(LogTemp, Error, TEXT("MulticastHidePlayerHUDsRPC: PlayerHUD cast failed for PC %s (HUD Class: %s)"),
					*PC->GetName(), *RawHUD->GetClass()->GetName());
				continue;
			}*/

			// BossHpBarWidget이 유효한지 확인 후 숨기기
			if (IsValid(PlayerHUD->BossHpBarWidget))
			{
				PlayerHUD->HideBossHealthBar(this);
				//UE_LOG(LogTemp, Warning, TEXT("MulticastHidePlayerHUDsRPC: Hidden BossHpBarWidget for PC %s"), *PC->GetName());
			}
			/*else
			{
				UE_LOG(LogTemp, Error, TEXT("MulticastHidePlayerHUDsRPC: BossHpBarWidget is nullptr for PC %s"), *PC->GetName());
			}*/
		}
	}
	/*else
	{
		UE_LOG(LogTemp, Warning, TEXT("MulticastHidePlayerHUDsRPC called on a non-server instance"));
	}*/
}

float ABoss_Mermaid::GetAIPatrolRadius()
{
	return 800.0f; // 어트리뷰트 세트로 변경 예정
}

float ABoss_Mermaid::GetAIDetectRange()
{
	return 3000.0f; // 어트리뷰트 세트로 변경 예정
}

float ABoss_Mermaid::GetAIAttackRange()
{
	return BossAttributeSet->GetAttackRange();
}

float ABoss_Mermaid::GetAITurnSpeed()
{
	return 2.0f; // 어트리뷰트 세트로 변경 예정
}
