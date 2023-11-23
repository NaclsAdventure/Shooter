// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter==nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		//��ý�ɫ��ǰ�ٶȣ�ָˮƽ�ϵ��ٶ�
		//��ɫ���ٶ���һ���ռ��е�����
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		speed = Velocity.Size();

		//��ɫ�Ƿ��ڿ���
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//��ɫ�Ƿ��ڼ���
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()>0.f)
		{
			bIsAccelarating = true;
		}
		else
		{
			bIsAccelarating = false;
		}

		//��׼(����)�������ת
		/**
		* Return the aim rotation for the Pawn.
		* If we have a controller, by default we aim at the player's 'eyes' direction
		* that is by default the Pawn rotation for AI, and camera (crosshair) rotation for human players.
		*/
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		//FString AimRotation = FString::Printf(TEXT("Base Aim Rotation:%f"), AimRotator.Yaw);

		//�ƶ��������ת
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		//FString MovementRotation = FString::Printf(TEXT("Movement Rotation:%f"), MovementRotator.Yaw);
		
		//������ߵ�ƫ����
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		//FString MovementOffsetMessage = FString::Printf(TEXT("Movement Offset:%f"), MovementOffsetYaw);

		if (ShooterCharacter->GetVelocity().Size()>0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		
		//if (GEngine)
		//{
		//	//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, AimRotation);
		//	//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, MovementRotation);
		//	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, MovementOffsetMessage);

		bIsAiming = ShooterCharacter->GetbAming();
	}
}
