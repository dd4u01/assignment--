#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

// 필요한 헤더를 불러오는 대신 이런게 있다고 선언만 해둘 수 있음!! 파일이 무거워지는 것을 방지 가능.
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT1_2_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()
	


public:
	ASpartaCharacter();

	// 스프링 암 컴포넌트(카메라 지지대/삼각대)
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category="Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount, // 데미지 양
		struct FDamageEvent const& DamageEvent, // 데미지 유형
		AController* EventInstigator,			// 데미지를 입힌 컴포넌트
		// 데미지를 일으킨 오브젝트
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& value); // 참조로 가져오는 이유는 큰 구조체로부터 가져오는 값의 부하를 줄이기 위해서..?
	UFUNCTION()
	void StartJump(const FInputActionValue& value); // 홀드의 기능 구분을 위해 분리 설정 권유
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverheadHP();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};
