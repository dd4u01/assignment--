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
	// �ʱ�ȭ
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

	// Ÿ�̸� ���� ����?
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimeHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true // true�� ����ؼ� �ݺ�
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

	// ��� ���������� ������
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No SpawnVolumes found in the level!"));
		return;
	}

	// ������ �� ������ ����
	const int32 ItemToSpawn = 40;
	int32 SpawnedItemCount = 0;
	bool bAtLeastOneCoinSpawned = false;

	// ������ ������ ����Ʈ
	TArray<FName> ItemNameList = {
		TEXT("SmallCoinItem"),
		TEXT("BigCoinItem"),
		TEXT("MineItem"),
		TEXT("HealingItem")
	};

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		// 1) ���� ������ ���� (DataTable���� ������)
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

		// 2) Ȯ�� ��� ���� ����
		float SpawnChance = SpawnRow->SpawnChance;
		if (FMath::RandRange(0.0f, 100.0f) > SpawnChance)
		{
			// Ȯ������ ���� ���� ������ �������� ����
			continue;
		}

		// 3) ���� �������� ����
		int32 RandomIndex = FMath::RandRange(0, FoundVolumes.Num() - 1);
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[RandomIndex]);
		if (!SpawnVolume)
		{
			continue;
		}

		// 4) ������ ����
		AActor* SpawnedActor = SpawnVolume->SpawnItem(SpawnRow->ItemClass);
		if (SpawnedActor)
		{
			SpawnedItemCount++;

			// 5) Ư�� �������� ��� �߰� ���� (��: ���� ī��Ʈ ����)
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

	// ������ ����
	SpawnItemsForWave();

	UpdateHUD();

	// �ð�(Ÿ�̸�)

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp, // �ð��� ������ ȣ��
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

			// ���̺� �ʱ�ȭ
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
	// Ÿ�̸� Ŭ����
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

		// ���� ���̺� �غ� (��: UI �ִϸ��̼�, ī��Ʈ�ٿ� ��), ������ ������ ������
		StartWave();
	}
	else
	{
		// ������ ���̺갡 ������ ���� ��ȯ �Ǵ� ���� �ܰ�� ����
		EndLevel();
	}
}

void ASpartaGameState::OnGameOver()
{
	// ����� or UI ����(����ȭ��?)
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
										// ��Ʈ���� Text�� ��ȯ
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