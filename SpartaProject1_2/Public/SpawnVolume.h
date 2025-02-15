#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT1_2_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

	UPROPERTY(VisibleAnyWhere, BluePrintReadOnly, Category="Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnyWhere, BluePrintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;
	UFUNCTION(BlueprintCallable, Category ="Spawning")
	AActor* SpawnRandomItem();

	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	FVector GetRandomPointInVolume() const;	// ·£´ý À§Ä¡

};
