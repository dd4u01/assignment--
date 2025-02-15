#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpartaPawn.generated.h"

class UCapsuleComponent;
class UArrowComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT1_2_API ASpartaPawn : public APawn
{
	GENERATED_BODY()

public:
	ASpartaPawn();

	// ������Ʈ
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Pawn")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Pawn")
	UArrowComponent* ArrowComp;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Pawn")
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// �̵� ���� ���� (������ GetCharacterMovement() ���� ������)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float MaxWalkSpeed = 600.f;

	// Pawn ȸ�� ���� (Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float TurnRate = 70.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Move")
	FVector Velocity;

	// --- �Է� �� ���� --- 
	// (������ GetActor___Vector() �� ������)
	// AddMovementInput()�� GetMovementComponent()�� ����ϴ� ���� ����..
	FVector2D MoveInput; // �̵� �Է� (X: ����, Y: �¿�)
	FVector2D LookInput; // Look �Է� (X: Yaw, Y: Pitch)

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value); // ������ �������� ������ ū ����ü�κ��� �������� ���� ���ϸ� ���̱� ���ؼ�..?
	// �̵��� 0�� ���� GetMovementComponent()�� �����߾����� ���� ����..
	void StopMove(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	void StopLook(const FInputActionValue& Value);
};
