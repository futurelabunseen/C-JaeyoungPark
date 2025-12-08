#pragma once

#include "CoreMinimal.h"
#include "Character/PPCharacter.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UI/PPGASPlayerStatusUserWidget.h" // 위젯 헤더 포함 필수!
#include "PPGASCharacter.generated.h"


UCLASS()
class PROPJECTPTEST_API APPGASCharacter : public APPCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	APPGASCharacter();

	FORCEINLINE virtual class UAnimMontage* GetSkillActionMontage() const { return SkillActionMontage; }
	FORCEINLINE virtual class UAnimMontage* GetDashActionMontage() const { return DashActionMontage; }
	FORCEINLINE virtual class UAnimMontage* GetComboActionMontage() const { return ComboActionMontage; }
	FORCEINLINE virtual class UAnimMontage* GetInteractionMontage() const { return InteractionMontage; }
	FORCEINLINE class UPPComboActionData* GetComboActionData() const { return ComboActionData; }

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void ZoomIn();
	void ZoomOut();

	//플레이어 입력 정의
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class AActor> InteractableItem;

	void ResetPlayer();

	// void DetectMonstersInRadius();

	FTimerHandle DeadTimerHandle;
	FTimerHandle InitializationTimerHandle;
	// FTimerHandle MemberTimerHandle;

	// 범위 설정 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float DetectionRadius;

	// 범위 내에 있는 몬스터들을 저장할 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detection")
	TArray<AActor*> DetectedMonsters;

	// [추가] 레벨업 처리를 위한 함수
	void LevelUp();

	// [추가] 현재 레벨을 가져오는 함수 (AttributeSet에서 필요할 수 있음)
	int32 GetLevel() const { return Level; }

protected:
	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	UFUNCTION()
	virtual void OnOutOfHealth();

	virtual void Tick(float DeltaTime);

	/*void MoveToStreamingLevel(const bool IsPlayerDeath);
	void UnloadMultipleStreamingLevels(const FName& LevelName);*/

protected:

	// GAS Section
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

	// [기존] 스탯 초기화용 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> InitStatEffect;

	// [추가] 레벨업 시 완전 회복용 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> LevelUpHealEffect;

	// [추가] 리플리케이션 함수 선언
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// [수정] 레벨 변수에 Replicated 속성 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Status")
	int32 Level = 1;

	// --------------------------------------------------------------

	// Character Section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> SkillActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DashActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> InteractionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPPComboActionData> ComboActionData;

	// --------------------------------------------------------------

	// Action Input Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ZoomInAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Interaction;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	//스프링암 길이 목표값
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SpringArm, meta = (AllowPrivateAccess = "true"))
	float ExpectedSpringArmLength = 800.0f;

	const float ZoomMinLength = 150.0f;
	const float ZoomMaxLength = 800.0f;

	//UPROPERTY()
	//TObjectPtr<class UPPGASWidgetComponent> HpBar;

	// [삭제] 더 이상 컴포넌트 방식은 쓰지 않습니다.
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	// class UPPGASWidgetComponent* HpBar;

	// [추가] 화면에 띄울 위젯 클래스 정보 (블루프린트 클래스)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// [추가] 실제로 화면에 떠 있는 위젯 인스턴스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPPGASPlayerStatusUserWidget> HUDWidget;

	// [추가] HUD 초기화용 헬퍼 함수
	void InitializeHUD();
};
