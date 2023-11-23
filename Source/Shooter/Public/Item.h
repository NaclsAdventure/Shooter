// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity :uint8 
{
	EIR_Damaged UMETA(DisplayName="Damaged"),
	EIR_Common UMETA(DisplayName="Common"),
	EIR_Uncommon UMETA(DisplayName="Uncommon"),
	EIR_Rare UMETA(DisplayName="Rare"),
	EIR_Legendary UMETA(DisplayName="Legendary"),

	EIR_MAX UMETA(DisplayName="DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState :uint8 
{
	EIS_Pickup UMETA(DisplayName="Pickup"),
	EIS_EquipInterping UMETA(DisplayName="EquipInterping"),
	EIS_Equipped UMETA(DisplayName="Equipped"),
	EIS_Pickedup UMETA(DisplayName="Pickedup"),
	EIS_Falling UMETA(DisplayName="Falling"),

	EIS_MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//当重叠时调用
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent*OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	//当重叠结束时调用
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent*OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	//根据不同稀有度设置TArray<bool> ActiveStars
	void SetActiveStars();

	void SetItemProperties(EItemState State);

	void FinishInterping();

	//当处于EquipInterping 状态时，启动Item 插值
	void ItemInterp(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* ItemMesh;

	//碰撞盒，用于射线检测等
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	class UBoxComponent* CollisionBox;
	
	//显示物品信息的控件
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* PickupWidget;

	//当与人物重叠时，开启物品的射线检测
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	class USphereComponent* AreaSphere;

	//显示在控件上的名字
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	FString ItemName;

	//物体数目
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	int32 ItemCount;

	//物品稀有度
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	TArray<bool> ActiveStars;

	//物品状态
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	EItemState ItemState;

	//Z方向上插值的曲线资产
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	class UCurveFloat* ItemZCurve;

	//插值的起始位置
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	FVector ItemInterpStartLocation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	FVector CameraTargetLocation;

	//插值时为真
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	bool bInterping;

	//开始插值时启动
	FTimerHandle ItemInterpTimer;

	//Timer和曲线的持续时间
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	float ZCurveTime;

	//timer中的回调函数是无参数的，故设置为成员变量，为回调函数存储
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* Character;

	//物体处于EquipInterping state 时每帧中存储当前物体X Y的插值
	float ItemInterpX;
	float ItemInterpY;

	//初始化相机和物品的Yaw的偏移
	float InterpInitialYawOffset;

	//用于缩放的曲线
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Item Properties",meta=(AllowPrivateAccess="true"))
	UCurveFloat* ItermScaleCurve;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const {return PickupWidget;}
	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE UBoxComponent* GetCollisionBox() const{ return CollisionBox; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE EItemState GetItemState()const { return ItemState; }
	void SetItemState(EItemState State);

	//设置插值用到的变量，启动插值计时器
	void StartItemCurve(AShooterCharacter* Char);
};
