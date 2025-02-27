#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Team8_Project/Damageable.h"
#include "BaseEnemy.generated.h"

class ASpawnVolume;
struct FAIStimulus;
class UBehaviorTree;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UFloatingPawnMovement;
class APatrolPath;
class ABaseEnemy;

DECLARE_MULTICAST_DELEGATE_OneParam(FEnemyDeathDelegate, ABaseEnemy*)

USTRUCT(BlueprintType)
struct FAttackPattern
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Anim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;
};

UCLASS()
class TEAM8_PROJECT_API ABaseEnemy : public APawn, public IDamageable
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	bool CanAttack();

	UFUNCTION(BlueprintCallable)
	bool CanAttackWithType(TSubclassOf<AActor> AttackType);
	bool CanAttackWithType(TSubclassOf<AActor> AttackType, TArray<FOverlapResult>& OutOverlapResults);
	
	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	bool IsAttacking();
	
	UFUNCTION(BlueprintCallable)
	void SetWaypointLocationToNext();

	UFUNCTION(BlueprintCallable)
	FVector GetWaypointLocation() const;

	void SetSpawnVolume(ASpawnVolume* Value);
	
	virtual float GetHP() const override;
	virtual void SetHP(float Value) override;

protected:
	virtual void BeginPlay() override;
	
	virtual void Death();
	
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	

private:
	float GetMaxAttackRange() const;
	void RemoveUnattackableActor(TArray<FOverlapResult>& OutOverlapResults, TSubclassOf<AActor> AttackType);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UFloatingPawnMovement> PawnMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UAIPerceptionComponent> AI_Perception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UAISenseConfig_Sight> AI_Sight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	FEnemyDeathDelegate OnDeath;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TArray<FAttackPattern> AttackPatterns;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DamagedMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHP = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RotationMul = 10.f;
	
	TObjectPtr<ASpawnVolume> SpawnVolume;
	int32 WaypointIndex;
};

