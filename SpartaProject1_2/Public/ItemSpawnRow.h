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

	// 레벨 구분용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 LevelIndex;

	// 웨이브 구분용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 WaveIndex;
};