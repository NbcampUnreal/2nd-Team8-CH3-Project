#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShopComponent.generated.h"

class ACH8_GameState;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM8_PROJECT_API UShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UShopComponent();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool AttemptItemPurchase(int32 ItemCost);

	ACH8_GameState* GameStateRef;

protected:
	virtual void BeginPlay() override;

};
