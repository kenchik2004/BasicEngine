#include "precompile.h"
#include "System/ObjBase.h"



bool ObjBase::changed_priority = true;

Object::Object()
{
	status.obj_type = ObjStat::NORMAL;
}

void Object::DebugDraw()
{
	DrawLine3D(cast(transform->position), cast(transform->position + transform->AxisX()), BLUE);
	DrawCone3D(cast(transform->position + transform->AxisX()), cast(transform->position + transform->AxisX() * 0.7f), 0.05f, 8, BLUE, BLUE, true);
	DrawLine3D(cast(transform->position), cast(transform->position + transform->AxisY()), GREEN);
	DrawCone3D(cast(transform->position + transform->AxisY()), cast(transform->position + transform->AxisY() * 0.7f), 0.05f, 8, GREEN, GREEN, true);
	DrawLine3D(cast(transform->position), cast(transform->position + transform->AxisZ()), RED);
	DrawCone3D(cast(transform->position + transform->AxisZ()), cast(transform->position + transform->AxisZ() * 0.7f), 0.05f, 8, RED, RED, true);
}

UIObject::UIObject()
{
	status.obj_type = ObjStat::UI; tag = UI; SetPriority(2000);
}
