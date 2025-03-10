// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionEffect.h"
#include "Team8_Project/MyCharacter.h"
#include "Team8_Project/Weapon/PlayerCombatComponent.h"
#include "Team8_Project/Damageable.h"

void UPotionEffect::ApplyItemEffect(AActor* Target)
{
    if (Target)
    {
        TScriptInterface<IDamageable> DamageInterface = Target;
        if (DamageInterface)
        {
           
            float MaxHealth = 100.f;
            float CurrentHealth = DamageInterface->GetHP();
            UE_LOG(LogTemp, Warning, TEXT("Damagable Interface in Potion Before  Get Hp : %f"), CurrentHealth);
            float Value = MaxHealth * (HealAmount / 100.f);
            float NewHealth = FMath::Min(CurrentHealth + Value, MaxHealth);

            DamageInterface->SetHP(NewHealth);
            AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Target);
            if (PlayerCharacter)
            {
                PlayerCharacter->GetCombatComponent()->UpdateHealth();
            }
            CurrentHealth = DamageInterface->GetHP();
            UE_LOG(LogTemp, Warning, TEXT("Damagable Interface in Potion After Get Hp : %f"), CurrentHealth);
        }
    }
}