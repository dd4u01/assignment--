#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	// 초기화
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 10.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 2;
	CurrentWaveIndex = 0;
	MaxWaves = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	// 타이머 갱신 관련?
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimeHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true // true면 계속해서 반복
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	} 
}

void ASpartaGameState::SpawnItemsForWave()
{
	if (!ItemSpawnTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemSpawnTable is not assigned!"));
		return;
	}

	// 모든 스폰볼륨을 가져옴
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No SpawnVolumes found in the level!"));
		return;
	}

	// 스폰할 총 아이템 개수
	const int32 ItemToSpawn = 40;
	int32 SpawnedItemCount = 0;
	bool bAtLeastOneCoinSpawned = false;

	// 스폰할 아이템 리스트
	TArray<FName> ItemNameList = {
		TEXT("SmallCoinItem"),
		TEXT("BigCoinItem"),
		TEXT("MineItem"),
		TEXT("HealingItem")
	};

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		// 1) 랜덤 아이템 선택 (DataTable에서 가져옴)
		FName SelectedItem = ItemNameList[FMath::RandRange(0, ItemNameList.Num() - 1)];
		FString RowNameString = FString::Printf(
			TEXT("L%d_W%d_%s"),
			CurrentLevelIndex + 1,
			CurrentWaveIndex + 1,
			*SelectedItem.ToString()
		);
		FName RowName(*RowNameString);

		FItemSpawnRow* SpawnRow = ItemSpawnTable->FindRow<FItemSpawnRow>(
			RowName,
			TEXT("SpawnItemsForWave"),
			true
		);

		if (!SpawnRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("Row not found: %s"), *RowNameString);
			continue;
		}

		// 2) 확률 기반 스폰 결정
		float SpawnChance = SpawnRow->SpawnChance;
		if (FMath::RandRange(0.0f, 100.0f) > SpawnChance)
		{
			// 확률보다 높은 값이 나오면 스폰하지 않음
			continue;
		}

		// 3) 랜덤 스폰볼륨 선택
		int32 RandomIndex = FMath::RandRange(0, FoundVolumes.Num() - 1);
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[RandomIndex]);
		if (!SpawnVolume)
		{
			continue;
		}

		// 4) 아이템 스폰
		AActor* SpawnedActor = SpawnVolume->SpawnItem(SpawnRow->ItemClass);
		if (SpawnedActor)
		{
			SpawnedItemCount++;

			// 5) 특정 아이템의 경우 추가 로직 (예: 코인 카운트 증가)
			if (SpawnedActor->IsA(ACoinItem::StaticClass()))
			{
				SpawnedCoinCount++;
			}

			UE_LOG(LogTemp, Log, TEXT("Spawned Item: %s at %s"),
				*SpawnRow->ItemName.ToString(),
				*SpawnVolume->GetName()
			);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Total Spawned Items: %d"), SpawnedItemCount);
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	UpdateHUD();

	StartWave();

	UE_LOG(LogTemp, Warning, TEXT("Level %d"), CurrentLevelIndex + 1);
}

void ASpartaGameState::StartWave()
{

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 아이템 스폰
	SpawnItemsForWave();

	UpdateHUD();

	// 시간(타이머)

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp, // 시간이 끝나면 호출
		WaveDuration,
		false);

	UE_LOG(LogTemp, Warning, TEXT("Wave %d Comming!, %d Coins Spawned"),
		CurrentWaveIndex + 1,
		SpawnedCoinCount);
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}

}

void ASpartaGameState::EndLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			// 웨이브 초기화
			CurrentWaveIndex = 0;
			SpartaGameInstance->CurrentWaveIndex = 0;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::EndWave()
{
	// 타이머 클리어
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentWaveIndex++;
			SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;
		}
	}

	if (CurrentWaveIndex < MaxWaves)
	{

		// 다음 웨이브 준비 (예: UI 애니메이션, 카운트다운 등), 딜레이 있으면 좋을듯
		StartWave();
	}
	else
	{
		// 마지막 웨이브가 끝나면 레벨 전환 또는 다음 단계로 진행
		EndLevel();
	}
}

void ASpartaGameState::OnGameOver()
{
	// 재시작 or UI 열기(메인화면?)
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
										// 스트링을 Text로 변환
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndextText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndextText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex+1)));
				}

				if (UTextBlock* WaveIndextText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveIndextText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex + 1)));
				}
			}
		}
	}
}