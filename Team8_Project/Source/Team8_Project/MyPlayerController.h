#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;


UCLASS()
class TEAM8_PROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()


	
public:
	AMyPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UUserWidget* HUDWidgetInstance;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* SlowWalkingAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* AimingAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* InventoryToggleAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* ShopOpenAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* WeaponModifyingUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* WeaponModifyingActionArrowUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* WeaponModifyingActionArrowDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* WeaponModifyingActionArrowLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	UInputAction* WeaponModifyingActionArrowRight;

	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DisplayHUD();

	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);

	void InitHUDWeaponAmmo(int32 CurrentAmmo,int32 MaxAmmo);

	UInputMappingContext* GetMappingContext() const;
	
	class AWeaponCrosshairHUD* GetWeaponCrosshairHUD();
	
protected:
	 AWeaponCrosshairHUD* WeaponCrosshairHUD;
};
