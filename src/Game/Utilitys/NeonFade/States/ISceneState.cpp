#include "ISceneState.h"

namespace NeonFade {

	ISceneState::ISceneState(Scene* owner_scene_)
	{
		owner_scene = owner_scene_;
		name = "";
	}

	std::string ISceneState::GetChangeRequest()
	{
		for (auto& request : change_requests) {
			// 優先度順にチェック
			//  関数が設定されていて、かつ関数がtrueを返したら状態遷移要求とみなす
			if (request->function && request->function()) {
				return request->state_name;
			}
		}
		return "";
	}

	void ISceneState::RegisterChangeRequest(const std::string& state_name, std::function<bool()>& func, u8 priority)
	{
		auto request = make_safe_unique<FunctionRequest>();
		request->function = std::move(func);
		request->priority = priority;
		request->state_name = state_name;
		// 優先度順に挿入
		auto insert_pos = std::find_if(change_requests.begin(), change_requests.end(),
			[priority](const SafeUniquePtr<FunctionRequest>& req) {
				return req->priority < priority;
			});
		change_requests.insert(insert_pos, std::move(request));

	}

}