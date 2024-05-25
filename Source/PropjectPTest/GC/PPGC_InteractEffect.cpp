// Fill out your copyright notice in the Description page of Project Settings.


#include "GC/PPGC_InteractEffect.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

UPPGC_InteractEffect::UPPGC_InteractEffect()
{

}

bool UPPGC_InteractEffect::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	const AActor* Instigator = Parameters.EffectContext.GetInstigator();
	if (Instigator)
	{
		UGameplayStatics::SpawnEmitterAtLocation(Instigator, ParticleSystem, Instigator->GetActorLocation(), FRotator::ZeroRotator, true);
	}
	else
	{
		return false;
	}

	return false;
}
