// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//前后移动
	void MoveForward(float value);
	
	//左右移动
	void MoveRight(float value);
	
	//方向键 相机 转向,抬头
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	//鼠标...
	void Turn(float Value);
	void LookUp(float Value);

	//开枪
	void FireWeapon();

	/*
	* @param MuzzleSocketLoction 枪口插槽位置
	* @param OutBeamLocation 射线检测位置
	*/
	bool GetBeamEndLocation(const FVector& MuzzleSocketLoction, FVector& OutBeamLocation);

	//设置bAiming值
	void AimingButtonPressed();
	void AimingButtonReleased();

	//没帧内插值更新当前相机视野值
	void CameraInterpZoom(float DeltaTime);

	//设置 方向键 控制相机转向 速率
	void SetLookRates();

	//计算十字准星缩放速率乘积因子
	void CalculateCrosshairSpread(float DeltaTime);

	//启动开枪冷却计时器，用于设置bFireBullet,控制开枪期间十字准星范围变化
	void StartCrosshairBulletFire();

	//Timer的回调函数需用UFUNCTION声明
	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();

	void FireButtonReleased();

	//启动开枪计时器，结束后回调函数AutoFireReset
	void StartFireTimer();

	//Timer句柄的回调函数，必须用UFUNCTION宏声明,启动下一轮开枪计时
	UFUNCTION()
	void AutoFireReset();

	//十字准星开始对可拾取物件的射线检测
	bool TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation);

	//当OverlappedItemCount>0时，射线追踪物体
	void TraceForItems();

	//生成默认武器并装备
	class AWeapon* SpawnDefaultWeapon();

	//装备武器
	void EquipWeapon(AWeapon* WeaponToEquip);

	//丢掉装备，装备落到地上
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	//丢掉当前武器，并装备当前追踪的武器，TraceHitItem
	void SwapWeapon(AWeapon* WeaponToSwap);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//我们倾向于将变量设为private,在提供public 的Getter Setter
private:

	//相机弹簧臂
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//相机
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	class UCameraComponent  * FollowCamera;

	//方向键、摇杆控制视角转向基础速率因子
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	//
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	//
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	//腰射视角转向基础速率因子
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	//瞄准射击视角转向基础速率因子
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	//瞄准 视角转向速率
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	//腰射 鼠标控制视角转向速率乘积因子，范围[0,1]
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"),meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float MouseHipTrunRate;

	//...
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"),meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float MouseHipLookUpRate;

	//瞄准射击 鼠标控制视角转向速率乘积因子，范围[0,1]
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"),meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float MouseAimingTurnRate;

	//...
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Camera,meta = (AllowPrivateAccess = "true"),meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float MouseAimingLookUpRate;

	//随机枪声
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Combat,meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//枪口闪光粒子效果
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Combat,meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//命中粒子效果
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Combat,meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImapctParticles;

	//烟雾轨迹效果
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Combat,meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BeamParticles;

	//开枪后坐力动画
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Movement,meta=(AllowPrivateAccess ="true"))
	class UAnimMontage* HipFireMontage;

	//处于瞄准时为真
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Combat,meta=(AllowPrivateAccess="true"))
	bool bAiming;

	//相机默认视角范围
	float CameraDefaultFOV;

	//瞄准时相机视角范围
	float CameraZoomFOV;

	//当前相机视角范围
	float CameraCurrentFOV;

	//相机视角变化插值速率
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Combat,meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	//十字准星缩放速率控制因子
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Crosshairs,meta=(AllowPrivateAccess="true"))
	float CrosshairSpreadMultiplier;

	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Crosshairs,meta=(AllowPrivateAccess="true"))
	float CrosshairVelocityFactor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Crosshairs,meta=(AllowPrivateAccess="true"))
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Crosshairs,meta=(AllowPrivateAccess="true"))
	float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Crosshairs,meta=(AllowPrivateAccess="true"))
	float CrosshairShootingFactor;

	//按下开火键时为真
	bool bFireButtonPressed;

	//开枪冷却完成时为真
	bool bShouldFire;

	//连续开火速率，即开启间间隔时间
	float AutomaticFireRate;

	//连续开火开枪间隔定时器句柄
	FTimerHandle AutoFireTimer;

	//连续开火的冷却时间
	float ShootTimeDuration;
	//bFireBullet 开火计时器启动时，设为真；计时期间不允许再次开火，计时结束（ShootTimeDuration）后为假
	bool bFireBullet;

	//连续开火调用修改十字准星的句柄
	FTimerHandle CrosshairShootTimer;

	//当我们每帧中需要追踪物体的时候为真
	bool bShouldTraceItems;

	//重叠物体的数量
	int8 OverlappedItemCount;

	//存储上一帧物体的指针
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Items,meta=(AllowPrivateAccess="true"))
	class AItem* TraceHitItemLastFrame;

	//当前装备的武器
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Combat,meta=(AllowPrivateAccess="true"))
	AWeapon* EquippedWeapon;

	//设置默认武器类, 转换成Aweapon  TSubclassof 提供一个UClass 类型安全性类型转换
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Combat,meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	//当前追踪的Item
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Combat,meta=(AllowPrivateAccess="true"))
	AItem* TraceHitItem;

	//前方
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Items,meta=(AllowPrivateAccess="true"))
	float CameraInterpDistance;

	//预期插值在相机位置上方的偏移距离
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Items,meta=(AllowPrivateAccess="true"))
	float CameraInterpElevation;

public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetbAming() const { return bAiming; }

	/*
	* 内联函数无法被蓝图调用
	FORCEINLINE bool GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; }
	*/
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount()const { return OverlappedItemCount; }

	//+或-当前重叠物体数量，更新bShouldTraceForItems
	void IncrementOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);
};
