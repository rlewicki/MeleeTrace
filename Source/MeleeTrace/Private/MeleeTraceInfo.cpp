// Copyright 2023, Robert Lewicki, All rights reserved.


#include "MeleeTraceInfo.h"

#include "MeleeTraceShape.h"

FMeleeTraceInfo::FMeleeTraceInfo()
{
	TraceShape = UMeleeTraceShape_Line::MakeLineShape();
}
