
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
	// ������ ���� ���
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
		// ���� ��ġ ����
		float RandomX = FMath::FRandRange(-50.f, 50.f);
		float RandomY = FMath::FRandRange(-50.f, 50.f);
		FVector2D RandomOffset = FVector2D(RandomX, RandomY);

		DamageTextBlock->SetText(FText::AsNumber(Damage));
		DamageTextBlock->SetVisibility(ESlateVisibility::Visible);

		// ��ġ ����
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DamageTextBlock->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(RandomOffset);
		}

		// ���� �ð� �Ŀ� ������ �ؽ�Ʈ �����
		if (GetWorld()->GetTimerManager().IsTimerActive(DamageTextTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(DamageTextTimerHandle);
		}

		GetWorld()->GetTimerManager().SetTimer(DamageTextTimerHandle, FTimerDelegate::CreateLambda([this]() {
			if (DamageTextBlock)
			{
				DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			}), 0.5f, false); // 1�� �Ŀ� �����
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
			if (OriginalMaterials.Num() == 0) // ���ʷ� ����� ���� ���� ��Ƽ���� ����
			{
				OriginalMaterials.SetNum(NumMaterials);
				for (int32 i = 0; i < NumMaterials; ++i)
				{
					OriginalMaterials[i] = MeshComp->GetMaterial(i); // ���� ��Ƽ���� ����
				}
			}

			for (int32 i = 0; i < NumMaterials; ++i)
			{
				MeshComp->SetMaterial(i, OverlayMaterial); // �������� ��Ƽ���� ����
			}

			// ���� Ÿ�̸Ӱ� ������ ����ϰ� ���ο� Ÿ�̸� ����
			if (GetWorld()->GetTimerManager().IsTimerActive(OverlayTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(OverlayTimerHandle);
			}

			GetWorld()->GetTimerManager().SetTimer(OverlayTimerHandle, this, &UPPGASDamageTextUserWidget::ResetMaterial, 0.1f, false); // 1�� �Ŀ� ����
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
					MeshComp->SetMaterial(i, OriginalMaterials[i]); // ���� ��Ƽ����� ����
				}
			}
		}
		// ��Ƽ���� ������ �Ϸ�Ǿ����Ƿ� OriginalMaterials �迭�� ���ϴ�.
		OriginalMaterials.Empty();
	}
}

