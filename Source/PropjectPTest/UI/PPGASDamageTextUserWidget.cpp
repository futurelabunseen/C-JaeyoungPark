
#include "UI/PPGASDamageTextUserWidget.h"
#include "Attribute/BossAttributeSet.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Components/CanvasPanelSlot.h"
#include "Materials/MaterialInstanceDynamic.h"

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

	ShowDamageText(Damage);
	ApplyOverlayMaterial();
}

void UPPGASDamageTextUserWidget::ShowDamageText(float Damage)
{
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
		if (GetWorld()->GetTimerManager().IsTimerActive(DamageTextTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(DamageTextTimerHandle);
		}

		GetWorld()->GetTimerManager().SetTimer(DamageTextTimerHandle, FTimerDelegate::CreateLambda([this]() {
			if (DamageTextBlock)
			{
				DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			}), 0.5f, false); // 1초 후에 숨기기
	}
}

void UPPGASDamageTextUserWidget::ApplyOverlayMaterial()
{
	if (OwnerActor)
	{
		UMeshComponent* MeshComp = OwnerActor->FindComponentByClass<UMeshComponent>();
		if (MeshComp && OverlayMaterial)
		{
			int32 NumMaterials = MeshComp->GetNumMaterials();
			if (OriginalMaterials.Num() == 0) // 최초로 적용될 때만 원래 머티리얼 저장
			{
				OriginalMaterials.SetNum(NumMaterials);
				for (int32 i = 0; i < NumMaterials; ++i)
				{
					OriginalMaterials[i] = MeshComp->GetMaterial(i); // 원래 머티리얼 저장
				}
			}

			for (int32 i = 0; i < NumMaterials; ++i)
			{
				MeshComp->SetMaterial(i, OverlayMaterial); // 오버레이 머티리얼 적용
			}

			// 기존 타이머가 있으면 취소하고 새로운 타이머 설정
			if (GetWorld()->GetTimerManager().IsTimerActive(OverlayTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(OverlayTimerHandle);
			}

			GetWorld()->GetTimerManager().SetTimer(OverlayTimerHandle, this, &UPPGASDamageTextUserWidget::ResetMaterial, 0.1f, false); // 1초 후에 복귀
		}
	}
}

void UPPGASDamageTextUserWidget::ResetMaterial()
{
	if (OwnerActor)
	{
		UMeshComponent* MeshComp = OwnerActor->FindComponentByClass<UMeshComponent>();
		if (MeshComp)
		{
			for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
			{
				if (OriginalMaterials[i])
				{
					MeshComp->SetMaterial(i, OriginalMaterials[i]); // 원래 머티리얼로 복귀
				}
			}
		}
		// 머티리얼 복원이 완료되었으므로 OriginalMaterials 배열을 비웁니다.
		OriginalMaterials.Empty();
	}
}

