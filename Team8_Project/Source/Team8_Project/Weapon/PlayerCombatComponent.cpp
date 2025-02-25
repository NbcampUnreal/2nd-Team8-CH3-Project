// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCombatComponent.h"
#include "WeaponBase.h"
#include "../MyCharacter.h"
#include "Engine/SkeletalMeshSocket.h"// 소켓
#include "Kismet/GameplayStatics.h"//crosshair
#include "DrawDebugHelpers.h"
#include "../MyPlayerController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

UPlayerCombatComponent::UPlayerCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true; //디버그 틱
	PlayerCharacter = nullptr;
	EquippedWeapon = nullptr;
}

void UPlayerCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCombatComponent::TraceUnderCrosshairs에서 PlayerConroller nullptr"));
		return;
	}

	FVector2D ViewportSize = FVector2D::ZeroVector;
	if (PC->GetLocalPlayer()->ViewportClient)
	{
		PC->GetLocalPlayer()->ViewportClient->GetViewportSize(ViewportSize);
	}

	// 매개변수에게 뷰포트 값이붙음(OUT 매크로)

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);// 스크린 화면의 중앙

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld=
	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{// 뷰포트 정중앙에서 World의 한지점까지 deproject한 것이 성공했나
		FVector Start = CrosshairWorldPosition;

		// 카메라랑 플레이어 사이에 있는 무언가를 조준 카메라가 무시하기 위한 코드
		if (PlayerCharacter)
		{
			float DistanceToCharcater = (PlayerCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharcater + 100.f);
			
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH_FOR_CROSSHAIR; // 시작점 + 방향*거리

		// 캐릭터나 서브클래스를 무시하기 위해 Query Params 설정
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());  // 현재 플레이어(또는 해당 Actor) 무시

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility,
			QueryParams
		);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End; // 거리안에 닿는것이 없다면 마지막을
			HitTargetPos = End;
		}
		else
		{
			HitTargetPos = TraceHitResult.ImpactPoint;
		}

	}
}

void UPlayerCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (PlayerCharacter == nullptr || PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player 또는 Controller없음"));
		return;
	}
	PlayerController = PlayerController == nullptr ?
		Cast<AMyPlayerController>(PlayerCharacter->Controller) : PlayerController ;
	if (PlayerController)
	{
		PlayerCrosshairHUD = PlayerCrosshairHUD == nullptr ?
			Cast<AWeaponCrosshairHUD>(PlayerController->GetHUD()) : PlayerCrosshairHUD;
		if (PlayerCrosshairHUD)
		{

			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{// 무기 미장착시
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			// 조준선 퍼짐 결정하기
			// 걷기 상태의 퍼짐
			// 현재 걷기상태의 임의 값이 600이기 때문에 0~600을 0~1로 맵핑
			FVector2D WalkSpeedRange(0.f, PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = PlayerCharacter->GetVelocity();
			Velocity.Z = 0.f;
			CrosshairVelocityFactor =
				FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			// 현재 속도에 따른 0~1 사이의 범위가 정해져 이것이
			// HUD에 넘어갈구조체의 조준선 퍼짐 값으로 결정됨



			if (PlayerCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairinAirFactor
					= FMath::FInterpTo(CrosshairinAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CrosshairinAirFactor
					= FMath::FInterpTo(CrosshairinAirFactor, 0.f, DeltaTime, 30.f);
			}// 기본 값, 범위 값, 적용 시간, 적용 시간 수치(얼마나 빠르게)
			// 앉기도 구현하는가?
			
			
			if (bIsAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.58f , DeltaTime,30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}
			// 총을 쐈을 때 벌어지는 수치가 점점 0으로 줄어드는 방식
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime,40.f);

			HUDPackage.CrosshairsSpread =
				CrosshairBaseFactor	+ CrosshairVelocityFactor 
				+ CrosshairinAirFactor
				- CrosshairAimFactor
				+ CrosshairShootingFactor;

			PlayerCrosshairHUD->SetHUDPackage(HUDPackage);
		}
	
	}


}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController = Cast<AMyPlayerController>(PC);

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetFollowCamera())
		{
			DefaultFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		
	}
	
}

void UPlayerCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (PlayerCharacter == nullptr || WeaponToEquip == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("캐릭터 또는 장비 nullptr"));
		return;
	}
	// 소켓에 장비 장착
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWT_Equipped);

	
	const USkeletalMeshSocket* HandSocket =
		PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(PlayerCharacter);
	
}

void UPlayerCombatComponent::FireButtonPressed(bool bPressed)
{
	// 발사
	if (!EquippedWeapon) return;
	bFireButtonPressed = bPressed;
	// 애니메이션 재생
	if (PlayerCharacter&& bFireButtonPressed)
	{

		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);

		PlayerCharacter->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire(HitTargetPos);
	
		// 총을 쐈을 떄 벌어질 조준선에 크기에 더해질 값
		CrosshairShootingFactor = 0.75f;
	}



}

void UPlayerCombatComponent::SetAiming(bool _bIsAiming)
{
	if (PlayerCharacter == nullptr || EquippedWeapon == nullptr)
	{
		return;
	}

	bIsAiming = _bIsAiming;

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
	{
		PlayerCharacter->ShowSniperScopeWidget(bIsAiming);
	}


}

void UPlayerCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (bIsAiming)
	{
		CurrentFOV=FMath::FInterpTo(
			CurrentFOV,
			EquippedWeapon->GetWeaponZoomFov(),
			DeltaTime,EquippedWeapon->GetWeaponZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV,
			DefaultFOV,
			DeltaTime, ZoomInterpSpeed);
	}
	if (PlayerCharacter && PlayerCharacter->GetFollowCamera())
	{
		PlayerCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);
	InterpFOV(DeltaTime);
}

