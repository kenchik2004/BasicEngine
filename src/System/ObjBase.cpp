#include "precompile.h"
#include "System/ObjBase.h"



bool ObjBase::changed_priority = true;

Object::Object()
{
	status.obj_type = ObjStat::NORMAL;
}

Object::Object(std::string name_)
{
	status.obj_type = ObjStat::NORMAL;
	name = name_;
}

void Object::DebugDraw()
{
	DrawLine3D(cast(transform->position), cast(transform->position + transform->AxisX()), Color::BLUE);
	DrawCone3D(cast(transform->position + transform->AxisX()), cast(transform->position + transform->AxisX() * 0.7f), 0.05f, 8, Color::BLUE, Color::BLUE, true);
	DrawLine3D(cast(transform->position), cast(transform->position + transform->AxisY()), Color::GREEN);
	DrawCone3D(cast(transform->position + transform->AxisY()), cast(transform->position + transform->AxisY() * 0.7f), 0.05f, 8, Color::GREEN, Color::GREEN, true);
	DrawLine3D(cast(transform->position), cast(transform->position + transform->AxisZ()), Color::RED);
	DrawCone3D(cast(transform->position + transform->AxisZ()), cast(transform->position + transform->AxisZ() * 0.7f), 0.05f, 8, Color::RED, Color::RED, true);
}

UIObject::UIObject()
{
	status.obj_type = ObjStat::UI; tag = UI; SetPriority(2000);
}
