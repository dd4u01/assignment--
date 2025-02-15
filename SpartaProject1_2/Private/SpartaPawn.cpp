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
	SpringArmComp->TargetArmLength = 300.0f;	// 스프링 암 길이 설정
	// Pawn의 회전과 별개로 카메라가 독립적으로 회전하도록 함
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// 스프링 암 내부의 위치는 Socket 변수로 설정되어있음.
	// 이렇게 하면 스프링 암의 끝 부분에 카메라를 Setup 해준다.
	CameraComp->SetupAttachment(SpringArmComp);
	//CameraComp->bUsePawnControlRotation = false; // 마우스 무브먼트 시 카메라가 흔들리지 않게끔 설정(스프링 암이 이미 움직여줌)

	// 입력 값 초기화
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

	// 다양한 인풋에 대하여 EnhancedInputComponent에 맞춰 한 번 캐스팅을 해준다.
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction, // IA 호출
					ETriggerEvent::Triggered,	  // WASD 같은 키 입력 시..
					this,						  // 호출된 객체, Character
					&ASpartaPawn::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,	  // 마우스 이동 시
					this,
					&ASpartaPawn::Look
				);
			}
		}
	}
}

void ASpartaPawn::Move(const FInputActionValue& value)
{
	// 컨트롤러 유효 확인
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
