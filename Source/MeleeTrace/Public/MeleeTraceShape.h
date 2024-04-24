// Copyright 2024, Robert Lewicki, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionShape.h"
#include "UObject/Object.h"
#include "MeleeTraceShape.generated.h"

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class MELEETRACE_API UMeleeTraceShape : public UObject
{
	GENERATED_BODY()
public:
	virtual FCollisionShape CreateCollisionShape() const;
	virtual FQuat GetRotationOffset() const;
};

UCLASS()
class MELEETRACE_API UMeleeTraceShape_Line : public UMeleeTraceShape
{
	GENERATED_BODY()
public:
	virtual FCollisionShape CreateCollisionShape() const override;

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	static UMeleeTraceShape_Line* MakeLineShape();
};

UCLASS()
class MELEETRACE_API UMeleeTraceShape_Sphere : public UMeleeTraceShape
{
	GENERATED_BODY()
public:
	virtual FCollisionShape CreateCollisionShape() const override;

	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	static UMeleeTraceShape_Sphere* MakeSphereShape(float Radius);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Trace")
	float Radius = 1.0f;
};

UCLASS()
class MELEETRACE_API UMeleeTraceShape_Capsule : public UMeleeTraceShape
{
	GENERATED_BODY()
public:
	virtual FCollisionShape CreateCollisionShape() const override;
	virtual FQuat GetRotationOffset() const override;
	
	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	static UMeleeTraceShape_Capsule* MakeCapsuleShape(float Radius, float HalfHeight, const FRotator& RotationOffset);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Trace")
	float Radius = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Trace")
	float HalfHeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Trace")
	FRotator RotationOffset;
};

UCLASS()
class MELEETRACE_API UMeleeTraceShape_Box : public UMeleeTraceShape
{
	GENERATED_BODY()
public:
	virtual FCollisionShape CreateCollisionShape() const override;
	virtual FQuat GetRotationOffset() const override;
	
	UFUNCTION(BlueprintPure, Category = "Melee Trace")
	static UMeleeTraceShape_Box* MakeBoxShape(const FVector& Extent, const FRotator& RotationOffset);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Trace")
	FVector Extent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Trace")
	FRotator RotationOffset;
};