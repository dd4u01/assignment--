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

	// 컴포넌트
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

	// 이동 관련 변수 (원래는 GetCharacterMovement() 에서 가져옴)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float MaxWalkSpeed = 600.f;

	// Pawn 회전 감도 (Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float TurnRate = 70.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Move")
	FVector Velocity;

	// --- 입력 값 저장 --- 
	// (원래는 GetActor___Vector() 로 가져옴)
	// AddMovementInput()이 GetMovementComponent()를 사용하니 따로 구현..
	FVector2D MoveInput; // 이동 입력 (X: 전후, Y: 좌우)
	FVector2D LookInput; // Look 입력 (X: Yaw, Y: Pitch)

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value); // 참조로 가져오는 이유는 큰 구조체로부터 가져오는 값의 부하를 줄이기 위해서..?
	// 이동값 0에 대해 GetMovementComponent()로 구현했었으니 따로 구현..
	void StopMove(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	void StopLook(const FInputActionValue& Value);
};
