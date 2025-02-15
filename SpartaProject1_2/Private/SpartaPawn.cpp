#include "SpartaPawn.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASpartaPawn::ASpartaPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(false);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComp->SetupAttachment(RootComponent);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeltalMesh"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	SkeletalMeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;	// ������ �� ���� ����
	// Pawn�� ȸ���� ������ ī�޶� ���������� ȸ���ϵ��� ��
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// ������ �� ������ ��ġ�� Socket ������ �����Ǿ�����.
	// �̷��� �ϸ� ������ ���� �� �κп� ī�޶� Setup ���ش�.
	CameraComp->SetupAttachment(SpringArmComp);
	//CameraComp->bUsePawnControlRotation = false; // ���콺 �����Ʈ �� ī�޶� ��鸮�� �ʰԲ� ����(������ ���� �̹� ��������)

	// �Է� �� �ʱ�ȭ
	MoveInput = FVector2D::ZeroVector;
	LookInput = FVector2D::ZeroVector;
	Velocity = FVector::ZeroVector;
}

void ASpartaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpartaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �پ��� ��ǲ�� ���Ͽ� EnhancedInputComponent�� ���� �� �� ĳ������ ���ش�.
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction, // IA ȣ��
					ETriggerEvent::Triggered,	  // WASD ���� Ű �Է� ��..
					this,						  // ȣ��� ��ü, Character
					&ASpartaPawn::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,	  // ���콺 �̵� ��
					this,
					&ASpartaPawn::Look
				);
			}
		}
	}
}

void ASpartaPawn::Move(const FInputActionValue& value)
{
	// ��Ʈ�ѷ� ��ȿ Ȯ��
	if (!Controller) return;

	MoveInput = value.Get<FVector2D>();
}
void ASpartaPawn::StopMove(const FInputActionValue& Value)
{
	MoveInput = FVector2D::ZeroVector;
}
void ASpartaPawn::Look(const FInputActionValue& value)
{
	LookInput = value.Get<FVector2D>();

}
void ASpartaPawn::StopLook(const FInputActionValue& Value)
{
	LookInput = FVector2D::ZeroVector;
}
