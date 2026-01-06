#include "UI/PPGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UPPGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

UAbilitySystemComponent* UPPGASUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}
