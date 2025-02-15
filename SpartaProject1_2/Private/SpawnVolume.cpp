#include "SpawnVolume.h"
#include "Components/BoxComponent.h"


ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject <USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	// �ʱ�ȭ
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
	// �迭�� ��� �� ����
	ItemDataTable->GetAllRows(ContextString, AllRows); // ù ��° ���� - ���ڿ�(���� ����?)

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

	return nullptr; // ��� ���� ��ο� ���� �����ϰ� ó��

	// flow ����

	// 0.0 ~ 1.0
	// ������ 0.7

	// Item A 0.5 (50%)
	// Item B 0.8 (30%)
	// Item C 1.0 (������ 20%)
	// Item B�� ����
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// ������Ʈ�� ũ�� �������� (Vector ������)
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	// (200, 100, 50) (2,1,1) -> (400,100,50)
	// �߽ɿ��� �������� �Ÿ�

	FVector BoxOrigin = SpawningBox->GetComponentLocation(); // �߽� ��ǥ

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X), // �ڽ� x��ǥ ���� ������
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(), // �ι�° ���ڴ� ��ġ
		FRotator::ZeroRotator // ȸ���� �� ��.
	);

	return SpawnedActor;
}