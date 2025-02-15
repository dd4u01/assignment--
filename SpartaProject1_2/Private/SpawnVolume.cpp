#include "SpawnVolume.h"
#include "Components/BoxComponent.h"


ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject <USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	// 초기화
	ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	
	static const FString ContextString(TEXT("ItemSpawnContext"));
	// 배열에 모든 행 저장
	ItemDataTable->GetAllRows(ContextString, AllRows); // 첫 번째 인자 - 문자열(오류 추적?)

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;
	 
	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr; // 모든 제어 경로에 대해 안전하게 처리

	// flow 예시

	// 0.0 ~ 1.0
	// 랜덤값 0.7

	// Item A 0.5 (50%)
	// Item B 0.8 (30%)
	// Item C 1.0 (나머지 20%)
	// Item B가 나옴
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// 컴포넌트의 크기 가져오기 (Vector 값으로)
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// (200, 100, 50) (2,1,1) -> (400,100,50)
	// 중심에서 끝까지의 거리

	FVector BoxOrigin = SpawningBox->GetComponentLocation(); // 중심 좌표

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X), // 박스 x좌표 사이 랜덤값
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(), // 두번째 인자는 위치
		FRotator::ZeroRotator // 회전을 안 줌.
	);

	return SpawnedActor;
}