// Copyright 2024, Robert Lewicki, All rights reserved.

#include "MeleeTraceShape.h"

FCollisionShape FCollisionShape::LineShape;

FCollisionShape UMeleeTraceShape::CreateCollisionShape() const
{
	unimplemented();
	return FCollisionShape::LineShape;
}

FCollisionShape UMeleeTraceShape_Line::CreateCollisionShape() const
{
	return FCollisionShape::LineShape;
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

UMeleeTraceShape_Capsule* UMeleeTraceShape_Capsule::MakeCapsuleShape(float Radius, float HalfHeight)
{
	auto* NewShape = NewObject<UMeleeTraceShape_Capsule>();
	NewShape->Radius = Radius;
	NewShape->HalfHeight = HalfHeight;
	return NewShape;
}

FCollisionShape UMeleeTraceShape_Box::CreateCollisionShape() const
{
	return FCollisionShape::MakeBox(Extent);
}

UMeleeTraceShape_Box* UMeleeTraceShape_Box::MakeBoxShape(const FVector& Extent)
{
	auto* NewShape = NewObject<UMeleeTraceShape_Box>();
	NewShape->Extent = Extent;
	return NewShape;
}
