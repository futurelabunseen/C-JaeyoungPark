
#include "AI/Boss/BTSBoss_CheckHealthPoint.h"
#include "BossAI.h"
#include "AIController.h"
#include "Attribute/BossAttributeSet.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PPGASCharacterNonPlayer.h"

UBTSBoss_CheckHealthPoint::UBTSBoss_CheckHealthPoint()
{
	NodeName = TEXT("CheckHealthPoint");
	Interval = 1.0f;
}

void UBTSBoss_CheckHealthPoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// ControllingCharacter ĳ����
	if (const APPGASCharacterNonPlayer* const ControllingCharacter = Cast<APPGASCharacterNonPlayer>(OwnerComp.GetAIOwner()->GetPawn()); IsValid(ControllingCharacter))
	{
		// BossAttributeSet ��������
		if (ControllingCharacter->BossAttributeSet)
		{
			// ���� HP �ۼ�Ʈ ���
			CurrentHealthPointPercent = (ControllingCharacter->BossAttributeSet->GetHealth() / ControllingCharacter->BossAttributeSet->GetMaxHealth()) * 100.f;
			OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, CurrentHealthPointPercent);
			UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentHealthPointPercent);
			return;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, 0.f);
}