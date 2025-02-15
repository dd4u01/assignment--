#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Engine/DataTable.h"         // UDataTable
#include "SpartaGameState.generated.h"

UCLASS()
class SPARTAPROJECT1_2_API ASpartaGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Table")
	UDataTable* ItemSpawnTable;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Level")
	TArray<FName> LevelMapNames;

	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimeHandle;

	UFUNCTION(BluePrintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnItemsForWave();
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void StartWave();
	void OnWaveTimeUp();
	void OnCoinCollected();
	void EndWave();
	void EndLevel();
	void UpdateHUD();
};
