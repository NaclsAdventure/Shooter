// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"

// Sets default values
AShooterCharacter::AShooterCharacter():
	//方向键控制视角转向基础速率
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	//腰射和瞄准时方向键控制视角转向速率
	HipTurnRate(80.f),
	HipLookUpRate(80.f),
	AimingTurnRate(30.f),
	AimingLookUpRate(30.f),
	//鼠标控制视角转向速率缩放幅度
	MouseHipTrunRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.3f),
	MouseAimingLookUpRate(0.3f),
	bAiming(false),
	//相机视野
	CameraDefaultFOV(0.f),//beginplay时设置
	CameraZoomFOV(35.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	//十字准星缩放乘积因子
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	//连射子弹冷却时间
	ShootTimeDuration(0.05f),
	bFireBullet(false),
	//自动开火控制变量
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	//武器追踪变量
	bShouldTraceItems(false),
	TraceHitItemLastFrame(nullptr),
	//相机插值位置的偏移量
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//设置弹簧臂组件
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;//弹簧臂长度
	CameraBoom->bUsePawnControlRotation = true;	//使用控制器旋转控制弹簧臂旋转
	CameraBoom->SocketOffset = FVector(0.f,50.f,70.f);//设置弹簧臂上插槽位置偏移；弹簧臂插槽在弹簧臂的末端

	//设置相机组件
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);//将相机组件附加到弹簧臂插槽上
	FollowCamera->bUsePawnControlRotation = false;//相机不跟随控制器旋转，用弹簧臂控制相机

	//角色面向同步控制器Yaw方向，不同步Pitch Roll
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	//角色平滑转向到控制其预期方向，
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	//设置角色转向、跳跃、空中速度
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	EquipWeapon(SpawnDefaultWeapon());
}

void AShooterCharacter::MoveForward(float value)
{
	if ((Controller!=nullptr)&&value!=0.0f)
	{
		//获取控制器Yaw（XOY）上的旋转器
		const FRotator Rotation{ Controller->GetControlRotation()};
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };

		//获取旋转矩阵X轴方向，即前后方向
		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction,value);
	}
}

void AShooterCharacter::MoveRight(float value)
{
	if ((Controller != nullptr) && value != 0.0f)
	{
		//获取控制器Yaw（XOY）上的旋转器
		const FRotator Rotation{ Controller->GetControlRotation()};
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };
		//获取旋转矩阵Y轴方向，即左右方向
		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction,value);
	}

}

void AShooterCharacter::TurnAtRate(float Rate)
{
	//每帧转向
	AddControllerYawInput(Rate*BaseTurnRate*GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate*BaseLookUpRate*GetWorld()->GetDeltaSeconds());
}


void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	} 
	else
	{
		TurnScaleFactor = MouseHipTrunRate;
	}
	AddControllerYawInput(Value *TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	} 
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon==nullptr) return ;
	//播放枪声
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	//获取枪口插槽，后续作为用于射线检测起始点之一；作为枪口粒子效果播放位置
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		//获取枪口插槽Transform
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			//枪口处生成粒子效果
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		//烟雾轨迹终点，射线检测终点
		FVector BeamEndPoint;
		bool bGetbeamEndLocation = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEndPoint);
		if (bGetbeamEndLocation)
		{
			//命中粒子效果
			if (ImapctParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImapctParticles,BeamEndPoint);
			}

			//烟雾轨迹
			if (BeamParticles)
			{
				//从枪口播放烟雾轨迹
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
				if (Beam)
				{
					//设置烟雾轨迹粒子系统终点“Target”
					Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}
			}
		}
		
		//播放开枪后坐力动画
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance&&HipFireMontage)
		{
			AnimInstance->Montage_Play(HipFireMontage);
			AnimInstance->Montage_JumpToSection(FName("StartFire"));
		}

		StartCrosshairBulletFire();
	}

}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLoction, FVector& OutBeamLocation)
{

	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);
	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
		//OutBeamLocation 在TraceUnderCrosshairs设置为射线检查终点位置
	}
	
	//第二段射线检测，从枪口开始，检测枪口到命中地间是否有阻挡
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLoction};
	//第一段检测的距离是从碰撞地点到十字准星，第二段从枪口出发，因此检测终点应该比第一次命中地更远一点
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLoction };
	const FVector WeaponTraceEnd{ MuzzleSocketLoction+StartToEnd*1.25f };
	//const FVector EndFromBarrel{ (WeaponTraceEnd-WeaponTraceStart)*1.25f };

	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit, 
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	
	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	//如果处于瞄准状态，扩大相机视野
	if (bAiming)
	{
		//插值更新当前视野大小
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomFOV, DeltaTime, ZoomInterpSpeed);
	} 
	else
	{
		//插值更新当前视野大小
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	} 
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	//角色默认移动速度[0,600]
	FVector2D WalkSpeedRange{ 0.f,600.f };
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0;
	//按比例将WalkSpeedRange映射到VelocityMultiplierRange范围内
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,Velocity.Size());

	//如果角色在空中
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	//如果处于瞄准状态
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.25f, DeltaTime, 30.f);
	} 
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	//如果处于开枪状态
	if (bFireBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	} 
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	//十字准星缩放乘积因子
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor+
		CrosshairInAirFactor-
		CrosshairAimFactor+
		CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFireBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFireBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		//计时完成（连续开枪间隔），调用AutoFireReset，启动下一次开枪的计时器
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
	}
	
}

void AShooterCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation)
{
	//获取屏幕视口大小
	FVector2D ViewportSize;
	if (GEngine&&GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrossHairsScreenLocation{ ViewportSize.X / 2.0f,ViewportSize.Y / 2.0f };
	CrossHairsScreenLocation.Y -= 50.f;

	//转换为世界坐标
	FVector CrossHairsWorldLocation;
	FVector CrossHairsWorldDirection;

	bool bScreenToWorld =UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrossHairsScreenLocation,
		CrossHairsWorldLocation,
		CrossHairsWorldDirection
	);

	if (bScreenToWorld)
	{
		//射线检测
		FVector Start{ CrossHairsWorldLocation };
		FVector End{ Start + CrossHairsWorldDirection * 50'000.f };
		OutHitLocation = End;
	
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult,HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (TraceHitItem&&TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}
			//前一帧存储的HitItem
			if (TraceHitItemLastFrame)
			{
				//命中了与前一帧不同的Item
				// 或 本帧没有命中Item
				if (TraceHitItem!=TraceHitItemLastFrame)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			//为下一帧存储HitItem的指针
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	//没有重叠的物体
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
	
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		//生成武器  UClass* ：包含蓝图的变量
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		return DefaultWeapon;
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		//获得右手上的武器插槽
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			//将武器附加到插槽上
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		//auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
		//SwapWeapon(TraceHitWeapon);
		//先播放曲线，武器交换到StartItemCurve中完成
		TraceHitItem->StartItemCurve(this);
	}
}

void AShooterCharacter::SelectButtonReleased()
{

}

//丢弃原武器并装备新武器
void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//更新当前相机视野
	CameraInterpZoom(DeltaTime);

	//更新当前相机转向速率
	SetLookRates();

	//更新当前准星缩放倍率
	CalculateCrosshairSpread(DeltaTime);

	//检查重叠物体数目，然后最终重叠物体
	TraceForItems();

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//检测PlayerInputComponent是否有效
	check(PlayerInputComponent);

	//轴绑定
	PlayerInputComponent->BindAxis("MoveForward", this,&AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this,&AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this,&AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	//动作绑定
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton",IE_Pressed,this,&AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton",IE_Released,this,&AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Aiming",IE_Pressed,this,&AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Aiming",IE_Released,this,&AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount+Amount==0)
	{
		OverlappedItemCount = 0;
		bShouldTraceItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceItems = true;
	}
}

FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector cameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };

	//Desired = cameraWorldLocation + CameraForward*A+CameraRight*B
	return cameraWorldLocation+CameraForward*CameraInterpDistance+FVector(0.f,0.f,CameraInterpElevation);
}

//捡起并交换武器
void AShooterCharacter::GetPickupItem(AItem* Item)
{
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}
}


