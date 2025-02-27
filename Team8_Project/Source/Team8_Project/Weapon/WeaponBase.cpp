// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BulletCaseBase.h"
#include "Engine/SkeletalMeshSocket.h" // 탄피배출(BulletCase)
#include "../MyCharacter.h"
#include "../MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

AWeaponBase::AWeaponBase() :
	WeaponState(EWeaponState::EWT_Dropped),
	OwnerPlayerCharacter(nullptr),
	OwnerPlayerController(nullptr)
{
	WeaponType = EWeaponType::EWT_None;// 무기 없음초기화

	//Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	//SetRootComponent(Scene);

	// 스태틱 메시 컴포넌트 생성 및 설정
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(WeaponSkeletalMesh);
	
	// 모두 block pawn은 ignore
	WeaponSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	AreaSphere->SetupAttachment(WeaponSkeletalMesh);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnItemOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnItemEndOverlap);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SetWeaponState(WeaponState);
}

void AWeaponBase::SetWeaponState(EWeaponState CurWeaponState)
{
	WeaponState = CurWeaponState;
	switch (WeaponState)
	{
	case EWeaponState::EWT_Equipped:
		// ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponSkeletalMesh->SetSimulatePhysics(false);
		WeaponSkeletalMesh->SetEnableGravity(false);
		WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		WeaponSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		break;
	case EWeaponState::EWT_Dropped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponSkeletalMesh->SetSimulatePhysics(true);
		WeaponSkeletalMesh->SetEnableGravity(true);
		WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		WeaponSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		break;
	}
}

void AWeaponBase::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OwnerPlayerCharacter = Cast<AMyCharacter>(OtherActor);
	if (OwnerPlayerCharacter)
	{
		OwnerPlayerCharacter->SetPickableWeapon(this);
	}
}

void AWeaponBase::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OwnerPlayerCharacter = Cast<AMyCharacter>(OtherActor);
	if (OwnerPlayerCharacter)
	{
		OwnerPlayerCharacter->SetPickableWeapon(nullptr);
	}
}

void AWeaponBase::ActivateItem(AActor* Activator)
{
	// PickUpWidget
	
}

//FName AWeaponBase::GetItemType()
//{
//	return FName();
//}

void AWeaponBase::SpendRound()
{
	--CurrentWeaponAmmo;
	OwnerPlayerCharacter = OwnerPlayerCharacter == nullptr ? Cast<AMyCharacter>(GetOwner()) : OwnerPlayerCharacter;

	if (OwnerPlayerCharacter)
	{
		OwnerPlayerController =
			OwnerPlayerController == nullptr ? Cast<AMyPlayerController>(OwnerPlayerCharacter->Controller) : OwnerPlayerController;
		if (OwnerPlayerController)
		{
			OwnerPlayerController->SetHUDWeaponAmmo(CurrentWeaponAmmo);
		}
	}
}

void AWeaponBase::Dropped()
{
	SetWeaponState(EWeaponState::EWT_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponSkeletalMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	OwnerPlayerCharacter = nullptr	;
	OwnerPlayerController = nullptr;
}

FName AWeaponBase::GetItemType() const
{
	return FName(TEXT("Weapon"));
}

EWeaponType AWeaponBase::GetWeaponType() const
{
	return WeaponType;
}

int32 AWeaponBase::GetCurrrentWeaponAmmo() const
{
	return CurrentWeaponAmmo;
}

void AWeaponBase::SetCurrentWeaponAmmo(int32 _Ammo)
{
	CurrentWeaponAmmo = _Ammo;
}

int32 AWeaponBase::GetMaxWeaponAmmo() const
{
	return MaxWeaponAmmo;
}

void AWeaponBase::SetMaxWeaponAmmo(int32 _Ammo)
{
	MaxWeaponAmmo = _Ammo;
}

void AWeaponBase::Fire(const FVector& HitTarget)
{
	
	// 무기 발사
	if (FireAnimation)
	{
		// 스켈레탈 애니메이션을 반복하지 않고 재생
		WeaponSkeletalMesh->PlayAnimation(FireAnimation, false);
	}

	if (BulletCaseClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket =
			GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));

		if (AmmoEjectSocket)
		{
			// mesh로 부터 탄피배출 위치 소켓의 Transform 받기
			FTransform SocketTransform =
				AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ABulletCaseBase>(
					BulletCaseClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator());
			}
		}
	}

	SpendRound();
}

float AWeaponBase::GetWeaponZoomFov() const
{
	return ZoomedFOV;
}

float AWeaponBase::GetWeaponZoomInterpSpeed() const
{
	return ZoomInterpSpeed;
}

void AWeaponBase::Reload()
{
}
