#include "precompile.h"
#include "System/ObjBase.h"



bool ObjBase::changed_priority = true;

Object::Object()
{
	status.obj_type = ObjStat::NORMAL;
}

UIObject::UIObject()
{
	status.obj_type = ObjStat::UI; tag = UI; SetPriority(2000);
}
