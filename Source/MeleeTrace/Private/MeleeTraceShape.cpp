// Copyright 2024, Robert Lewicki, All rights reserved.

#include "MeleeTraceShape.h"

FCollisionShape UMeleeTraceShape::CreateCollisionShape() const
{
	unimplemented();
	return FCollisionShape();
}

FQuat UMeleeTraceShape::GetRotationOffset() const
{
	return FQuat::Identity;
}

FCollisionShape UMeleeTraceShape_Line::CreateCollisionShape() const
{
	return FCollisionShape();
}

UMeleeTraceShape_Line* UMeleeTraceShape_Line::MakeLineShape()
{
	auto* NewShape = NewObject<UMeleeTraceShape_Line>();
	return NewShape;
}

FCollisionShape UMeleeTraceShape_Sphere::CreateCollisionShape() const
{
	return FCollisionShape::MakeSphere(Radius);
}

UMeleeTraceShape_Sphere* UMeleeTraceShape_Sphere::MakeSphereShape(float Radius)
{
	auto* NewShape = NewObject<UMeleeTraceShape_Sphere>();
	NewShape->Radius = Radius;
	return NewShape;
}

FCollisionShape UMeleeTraceShape_Capsule::CreateCollisionShape() const
{
	return FCollisionShape::MakeCapsule(Radius, HalfHeight);
}

FQuat UMeleeTraceShape_Capsule::GetRotationOffset() const
{
	return RotationOffset.Quaternion();
}

UMeleeTraceShape_Capsule* UMeleeTraceShape_Capsule::MakeCapsuleShape(float Radius,
	float HalfHeight,
	const FRotator& RotationOffset)
{
	auto* NewShape = NewObject<UMeleeTraceShape_Capsule>();
	NewShape->Radius = Radius;
	NewShape->HalfHeight = HalfHeight;
	NewShape->RotationOffset = RotationOffset;
	return NewShape;
}

FCollisionShape UMeleeTraceShape_Box::CreateCollisionShape() const
{
	return FCollisionShape::MakeBox(Extent);
}

FQuat UMeleeTraceShape_Box::GetRotationOffset() const
{
	return RotationOffset.Quaternion();
}

UMeleeTraceShape_Box* UMeleeTraceShape_Box::MakeBoxShape(const FVector& Extent, const FRotator& RotationOffset)
{
	auto* NewShape = NewObject<UMeleeTraceShape_Box>();
	NewShape->Extent = Extent;
	NewShape->RotationOffset = RotationOffset;
	return NewShape;
}
