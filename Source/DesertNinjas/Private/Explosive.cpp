// Fill out your copyright notice in the Description page of Project Settings.


#include "../Source/DesertNinjas/Public/Explosive.h"


#include "../Source/DesertNinjas/DesertNinjasCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
// #include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
	Damage = 15.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("Begin overlap on explosive"));

	if (OtherActor)
	{
		ADesertNinjasCharacter* Main = Cast<ADesertNinjasCharacter>(OtherActor);
		/*AEnemy* Enemy = Cast<AEnemy>(OtherActor);*/
		if (Main)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fire on explosive bp"));
			OnExplosionBP(Main);
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
					OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);

			Destroy();
		}
	}
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("Begin overlap on explosive"));

	//UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverlapEnd()"));
}