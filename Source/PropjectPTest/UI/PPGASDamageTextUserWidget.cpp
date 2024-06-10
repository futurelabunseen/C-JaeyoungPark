
#include "UI/PPGASDamageTextUserWidget.h"
#include "Attribute/BossAttributeSet.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Components/CanvasPanelSlot.h"


void UPPGASDamageTextUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DamageTextBlock)
	{
		DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPPGASDamageTextUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);
	OwnerActor = InOwner;

	if (ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ASC->GetOwner()->GetName());
		ASC->GetGameplayAttributeValueChangeDelegate(UBossAttributeSet::GetHealthAttribute()).AddUObject(this, &UPPGASDamageTextUserWidget::SpawnDamage);
	}
}

void UPPGASDamageTextUserWidget::SpawnDamage(const FOnAttributeChangeData& ChangeData)
{
	// 데미지 값을 계산
	float Damage = ChangeData.OldValue - ChangeData.NewValue;
	if (Damage <= 0)
	{
		return;
	}

	if (DamageTextBlock)
	{
		// 랜덤 위치 설정
		float RandomX = FMath::FRandRange(-50.f, 50.f);
		float RandomY = FMath::FRandRange(-50.f, 50.f);
		FVector2D RandomOffset = FVector2D(RandomX, RandomY);

		DamageTextBlock->SetText(FText::AsNumber(Damage));
		DamageTextBlock->SetVisibility(ESlateVisibility::Visible);

		// 위치 설정
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DamageTextBlock->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(RandomOffset);
		}

		// 일정 시간 후에 데미지 텍스트 숨기기
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]() {
			if (DamageTextBlock)
			{
				DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			}), 1.0f, false); // 1초 후에 숨기기
	}
}
