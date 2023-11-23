// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	class AShooterCharacter* ShooterCharacter;

	//角色速度
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	float speed;

	//角色是否在空中
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	bool bIsInAir;

	//角色是否加速，与物理中的加速定义不同，游戏中用于判断是否移动，e.g匀速运动在物理中，加速度为0，在此术语中则不同
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	bool bIsAccelarating;

	//扫射时使用的yaw方向的偏移(移动方向和朝向方向的偏移)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	float MovementOffsetYaw;

	//停止移动前最后一帧的yaw方向的偏移
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	float LastMovementOffsetYaw;

	//角色是否处于瞄准（肩射）
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	bool bIsAiming;

};
