#include "precompile.h"
#include "System/ObjBase.h"
#include "System/Scene.h"
#include <algorithm>



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

void ObjBase::SyncComponentsPriority()
{
    if (dirty_priority_components.empty()) return;

    // 1) dirty_GetPriority()_components を優先度順に並べ替える
    std::sort(dirty_priority_components.begin(), dirty_priority_components.end(),
        [](ComponentP a, ComponentP b) { return a->GetPriority() < b->GetPriority(); });

    // 2) まとめて components へ挿入
    for (ComponentP& obj : dirty_priority_components)
    {
		if (obj->status.status_bit.is(CompStat::STATUS::REMOVED))
			continue;
        // 2‑1) 古い場所を消す（同じポインタ重複を防ぐ）
        auto cur = std::find(components.begin(), components.end(), obj);
        if (cur != components.end()) components.erase(cur);

        // 2‑2) 優先度に合った場所を二分探索で探す
        auto pos = std::upper_bound(components.begin(), components.end(), obj,
            [](ComponentP a, ComponentP b) { return a->GetPriority() < b->GetPriority(); });

        components.insert(pos, obj);  // ここに差し込む
    }
    dirty_priority_components.clear();
}

void ObjBase::SetPriority(int prio)
{ scene->SetObjPriority(prio, shared_from_this()); }
