﻿#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Inventory/InventoryType.h" 
#include "../BaseItem.h"
#include "ItemObject/ItemEffectBase.h"
#include "ConsumableItemRow.generated.h"

UENUM(BlueprintType)
enum class EBuffType : uint8
{
    Buff,
    Debuff,
    None
};

USTRUCT(BlueprintType)
struct FConsumableItemRow : public FTableRowBase
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemKey = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemName = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType = EItemType::Consumable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* ItemImage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TSubclassOf<UItemEffectBase> ItemEffectClass;

    UPROPERTY(EditAnywhere, Category = "Item")
    EBuffType BuffType;

    UPROPERTY(EditAnywhere, Category = "Item")
    float Amount;

    UPROPERTY(EditAnywhere, Category = "Item")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float ItemSalePrice;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float ItemPurchasePrice;

   
};
