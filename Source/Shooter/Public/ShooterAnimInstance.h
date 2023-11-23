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

	//��ɫ�ٶ�
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	float speed;

	//��ɫ�Ƿ��ڿ���
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	bool bIsInAir;

	//��ɫ�Ƿ���٣��������еļ��ٶ��岻ͬ����Ϸ�������ж��Ƿ��ƶ���e.g�����˶��������У����ٶ�Ϊ0���ڴ���������ͬ
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	bool bIsAccelarating;

	//ɨ��ʱʹ�õ�yaw�����ƫ��(�ƶ�����ͳ������ƫ��)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	float MovementOffsetYaw;

	//ֹͣ�ƶ�ǰ���һ֡��yaw�����ƫ��
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	float LastMovementOffsetYaw;

	//��ɫ�Ƿ�����׼�����䣩
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	bool bIsAiming;

};
