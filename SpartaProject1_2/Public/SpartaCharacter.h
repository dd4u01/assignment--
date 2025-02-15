#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

// �ʿ��� ����� �ҷ����� ��� �̷��� �ִٰ� ���� �ص� �� ����!! ������ ���ſ����� ���� ���� ����.
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

	// ������ �� ������Ʈ(ī�޶� ������/�ﰢ��)
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
		float DamageAmount, // ������ ��
		struct FDamageEvent const& DamageEvent, // ������ ����
		AController* EventInstigator,			// �������� ���� ������Ʈ
		// �������� ����Ų ������Ʈ
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& value); // ������ �������� ������ ū ����ü�κ��� �������� ���� ���ϸ� ���̱� ���ؼ�..?
	UFUNCTION()
	void StartJump(const FInputActionValue& value); // Ȧ���� ��� ������ ���� �и� ���� ����
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
