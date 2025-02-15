#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
// ���������� ����ϴ� cpp���� ��� �ҷ�����
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	// ��� ���� ���� false��
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;	// ������ �� ���� ����
	SpringArmComp->bUsePawnControlRotation = true;	// ���콺 �����Ʈ�� ���� ��ġ ���� ����

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// ������ �� ������ ��ġ�� Socket ������ �����Ǿ�����.
	// �̷��� �ϸ� ������ ���� �� �κп� ī�޶� Setup ���ش�.
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // ���콺 �����Ʈ �� ī�޶� ��鸮�� �ʰԲ� ����(������ ���� �̹� ��������)

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	// ��ũ����� or ������ (���̴� �ϴ� ����� ���� 2D / 3D�� ��������)
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&ASpartaCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,	  // Space
					this,
					&ASpartaCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,	  // Space �Է��� ������
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,	  // ���콺 �̵� ��
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,	  // shift
					this,
					&ASpartaCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,	  // shift �Է� ����
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	// ��Ʈ�ѷ� ��ȿ Ȯ��
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	
	// float �ε��Ҽ����� �ޱ⿡ �Է°��� 0�� �ٻ��ϸ� �������� ���� ������ ����
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// �����Ǿ��ִ� �̵� �Լ� ���
		// GetActorForwardVector()�� ���� ���� ����
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}
void ASpartaCharacter::StartJump(const FInputActionValue& value)
{

	if (value.Get<bool>())
	{
		// ���� ���� �Լ� Jump() ���
		Jump();
	}
}
void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		// ���� ���� �Լ� StopJumping() ���
		StopJumping();
	}
}
void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	// ���� ���� �Լ�
	AddControllerYawInput(LookInput.X); // �¿� ȸ��
	AddControllerPitchInput(LookInput.Y); // ���� ȸ��
}
void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}
void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	
	UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth); // 0 ���Ϸ� �������� �ʰ�(�ּ�ġ), MaxHealth �̻� �ö��� �ʰ�(�ִ�ġ)
	UpdateOverheadHP();


	if (Health <= 0)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	// ���� ���� ����
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState < ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}

}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}