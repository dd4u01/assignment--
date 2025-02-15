#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	FName ItemName;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	TSubclassOf<AActor> ItemClass;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	float SpawnChance;

	// ���� ���п�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 LevelIndex;

	// ���̺� ���п�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 WaveIndex;
};