#include "precompile.h"
#include "System/SceneManager.h"
#include "System/Scene.h"
#include "System/DontDestroyOnLoadScene.h"
#include "System/Object.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/Camera.h"
#include "algorithm"
#include "System/Shader.h"
#include <System/Utils/Render.h>

#define SORT
bool USE_DEBUG_DRAW = false;


ScenePVec SceneManager::scenes = ScenePVec(0);			//!<シーンの配列
ScenePVec SceneManager::another_scenes = ScenePVec(1);	//!<裏シーンの配列
SceneP SceneManager::current_scene = nullptr;			//!<カレントシーン
std::vector<std::function<void()>> SceneManager::func_on_loop_finish(0);
SafeSharedPtr<Model> SceneManager::debug_box = nullptr;						//!<デバッグ描画用のボックスモデル

int be_default_cbuffer = -1;


#if 1
//----------------------------------------------------------------
int SceneManager::Init()
{

	TextureManager::Init();
	ModelManager::Init();
	AudioManager::Init();
	//===============================//
	//デバッグ用ボックス(SooS提供)のロード
	ModelManager::LoadAsModel("data/DebugBox/DebugMode/Box.mv1", "debug_box");
	debug_box = ModelManager::CloneModelByName("debug_box");
	//===============================//

	//レンダラー用のデフォルト定数バッファの作成
	be_default_cbuffer = CreateShaderConstantBuffer(sizeof(Vector4));

	//裏シーンの配列の0番目は必ずここで作成するシーンが入っている
	auto default_dontdestroyonload_scene = make_safe_shared<DontDestroyOnLoadScene>();
	default_dontdestroyonload_scene->Construct();
	another_scenes[0] = default_dontdestroyonload_scene;

	return 0;
}

void SceneManager::PreUpdate()
{
	if (Input::GetKeyDown(KeyCode::F4)) {
		USE_DEBUG_DRAW = !USE_DEBUG_DRAW;
	}
	//全ての裏シーンに対してPreUpdate
	for (auto& another_scene : another_scenes)
	{
		//先に、オブジェクトの優先度の同期を行う
		if (another_scene->objects.size() > 1) {
			another_scene->SyncGameObjectsPriority();

		}
		//次に、オブジェクト毎にコンポーネントの優先度の同期を行う
		for (auto& obj : another_scene->objects) {
			if (obj->components.size() > 1) {
				obj->SyncComponentsPriority();
			}
		}
		//全て終わった後にシーンのPreUpdate
		try {
			another_scene->PreUpdate();

		}
		catch (Exception& ex) {
			ex.Show();
		}

		//オブジェクトとコンポーネントのPreUpdate
		//オブジェクトが多くなると、vectorの再確保が多くなり、パフォーマンスが落ちるので、reserveで事前に確保しておく
		ObjectWPVec objs;
		objs.reserve(100);
		ComponentWPVec comps;
		objs.reserve(100);
		//シーンのオブジェクトを取得
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);
		//オブジェクト毎にPreUpdateを行う
		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
			//オブジェクトがnullptrなら何もしない
			if (!obj)
				return;
			//オブジェクトのステータスがアクティブならPreUpdateを行う
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->PreUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				//オブジェクトのコンポーネントを取得
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				//コンポーネント毎にPreUpdateを行う
				std::for_each(comps.begin(), comps.end(), [&obj](ComponentWP& comp) {
					//コンポーネントがnullptrなら何もしない
					if (!comp)
						return;
					//コンポーネントのステータスがアクティブならPreUpdateを行う
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
						try {
							comp->PreUpdate();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
					});
			}
			//コンポーネントの配列をクリア
			comps.clear();
			});
		//PreUpdateが終わったら、シーンのPreUpdateFinishを呼ぶ
		another_scene->OnPreUpdateFinish();
	}

	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	//カレントシーンのオブジェクトの優先度を同期
	if (current_scene->objects.size() > 1) {
		current_scene->SyncGameObjectsPriority();
	}
	//カレントシーンのオブジェクト毎にコンポーネントの優先度を同期
	for (auto& obj : current_scene->objects) {
		if (obj->components.size() > 1) {
			obj->SyncComponentsPriority();
		}
	}

	//カレントシーンのPreUpdateを呼ぶ
	current_scene->PreUpdate();

	//オブジェクトとコンポーネントのPreUpdate
	//オブジェクトが多くなると、vectorの再確保が多くなり、パフォーマンスが落ちるので、reserveで事前に確保しておく
	ObjectWPVec objs;
	ComponentWPVec comps;
	objs.reserve(100);
	comps.reserve(100);

	//カレントシーンのオブジェクトを取得
	for (auto& obj : current_scene->GetGameObjectPtrVec<::Object>())
		objs.push_back(obj);

	//オブジェクト毎にPreUpdateを行う
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		//オブジェクトがnullptrなら何もしない
		if (!obj)
			return;
		//オブジェクトのステータスがアクティブならPreUpdateを行う
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			//オブジェクトのPreUpdateを行う
			obj->PreUpdate();
			//オブジェクトのコンポーネントを取得
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			//コンポーネント毎にPreUpdateを行う
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				//コンポーネントがnullptrなら何もしない
				if (!comp)
					return;
				//コンポーネントのステータスがアクティブならPreUpdateを行う
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PreUpdate();
				});
		}
		//コンポーネントの配列をクリア
		comps.clear();
		});
	//PreUpdateが終わったら、カレントシーンのPreUpdateFinishを呼ぶ
	current_scene->OnPreUpdateFinish();
}

void SceneManager::Update()
{

	for (auto& another_scene : another_scenes)
	{
		another_scene->Update();

		ObjectWPVec objs;
		ComponentWPVec comps;
		objs.reserve(100);
		comps.reserve(100);
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);

		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {

			if (!obj)
				return;

			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->Update();
				}
				catch (Exception& ex) {
					ex.Show();
				}

				for (auto& comp : obj->components) {
					comps.push_back(comp);
				}

				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
							try {
								comp->Update();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			comps.clear();
			});
		another_scene->OnUpdateFinish();
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	try {
		current_scene->Update();
	}
	catch (Exception& ex) {
		ex.Show();
	}
	ObjectWPVec objs;
	ComponentWPVec comps;
	objs.reserve(100);
	comps.reserve(100);
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->Update();
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->Update();
				});
		}
		comps.clear();
		});
	current_scene->OnUpdateFinish();
}

void SceneManager::LateUpdate()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->LateUpdate();
		ObjectWPVec objs;
		ComponentWPVec comps;
		objs.reserve(100);
		comps.reserve(100);
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->LateUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				for (auto& comp : obj->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						{
							try {
								comp->LateUpdate();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			comps.clear();
			});
		another_scene->OnLateUpdateFinish();
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->LateUpdate();
	ObjectWPVec objs;
	ComponentWPVec comps;
	objs.reserve(100);
	comps.reserve(100);
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->LateUpdate();
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->LateUpdate();
				});
		}
		comps.clear();
		});
	current_scene->OnLateUpdateFinish();
}

void SceneManager::PostUpdate()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->PostUpdate();
		ObjectWPVec objs;
		ComponentWPVec comps;
		objs.reserve(100);
		comps.reserve(100);
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->PostUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				for (auto& comp : obj->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						{
							try {
								comp->PostUpdate();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			comps.clear();
			});
		another_scene->OnPostUpdateFinish();
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PostUpdate();
	ObjectWPVec objs;
	ComponentWPVec comps;
	objs.reserve(100);
	comps.reserve(100);
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostUpdate();
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PostUpdate();
				});
		}
		comps.clear();
		});
	current_scene->OnPostUpdateFinish();
}

void SceneManager::PrePhysics()
{

	for (auto& another_scene : another_scenes)
	{
		another_scene->PrePhysics();
		ObjectWPVec objs;
		ComponentWPVec comps;
		objs.reserve(100);
		comps.reserve(100);
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->PrePhysics();
				for (auto& comp : obj->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->PrePhysics();
					});
			}
			comps.clear();
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PrePhysics();
	ObjectWPVec objs;
	ComponentWPVec comps;
	objs.reserve(100);
	comps.reserve(100);
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PrePhysics();
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PrePhysics();
				});
		}
		});
}

void SceneManager::Physics()
{
	for (auto& another_scene : another_scenes)
	{
		ObjectWPVec objs;
		ComponentWPVec comps;
		objs.reserve(100);
		comps.reserve(100);
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->Physics();
				for (auto& comp : obj->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->Physics();
					});
			}
			comps.clear();
			});
		//Physicsだけ、シーンのシミュレーションを最後に行う
		another_scene->Physics();

	}

	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	ObjectWPVec objs;
	ComponentWPVec comps;
	objs.reserve(100);
	comps.reserve(100);
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->Physics();
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->Physics();
				});
		}
		comps.clear();
		});

	//Physicsだけ、シーンのシミュレーションを最後に行う
	current_scene->Physics();
}

void SceneManager::PostPhysics()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->PostPhysics();
		ObjectWPVec objs;
		ComponentWPVec comps;
		objs.reserve(100);
		comps.reserve(100);
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->PostPhysics();
				for (auto& comp : obj->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->PostPhysics();
					});
			}
			comps.clear();
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PostPhysics();
	ObjectWPVec objs;
	ComponentWPVec comps;
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostPhysics();
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PostPhysics();
				});
		}
		});
}

//シーン一つ一つに対してPreDraw,Draw,LateDrawを別々に呼ぶとカメラの情報が競合したり、
//いろいろとナンセンスなのでやめた。
void SceneManager::DrawCycleForOneScene(SceneP scene) {
	auto current_camera = scene->GetCurrentCamera();
	auto& active_cameras = scene->GetActiveCamerasRef();



	ObjectWPVec objs;
	ComponentWPVec comps;

	objs.reserve(100);
	comps.reserve(100);
	for (auto& obj : scene->GetGameObjectPtrVec<::Object>())
		objs.push_back(obj);
	for (auto& cam_wp : active_cameras) {
		if (auto cam = cam_wp.lock()) {
			if (!cam->owner->status.status_bit.is(ObjStat::STATUS::ACTIVE))
				continue;
			cam->SetCurrentCamera();


			scene->PreDraw();
			std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
				if (!obj)
					return;
				if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
					obj->PreDraw();
					for (auto& comp : obj->components) {
						comps.push_back(comp);
					}
					std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
						if (comp)
							if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
								comp->PreDraw();
						});
				}
				comps.clear();
				});
			scene->OnPreDrawFinish();
			scene->Draw();
			std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
				if (!obj)
					return;
				if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
					try {
						obj->Draw();
					}
					catch (Exception& ex) {
						ex.Show();
					}
					for (auto& comp : obj->components) {
						comps.push_back(comp);
					}
					std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
						if (comp)
							if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
							{
								try {
									comp->Draw();
								}
								catch (Exception& ex) {
									ex.Show();
								}
							}
						});
				}
				comps.clear();
				});
			scene->OnDrawFinish();

			scene->LateDraw();
			std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
				if (!obj)
					return;
				if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
					try {
						obj->LateDraw();
					}
					catch (Exception& ex) {
						ex.Show();
					}
					for (auto& comp : obj->components) {
						comps.push_back(comp);
					}
					std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
						if (comp)
							if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
							{
								try {
									comp->LateDraw();
								}
								catch (Exception& ex) {
									ex.Show();
								}
							}
						});
				}
				comps.clear();
				});
			scene->OnLateDrawFinish();
			if (USE_DEBUG_DRAW) {
				//デバッグ描画を行う


				SetUseLighting(false);
				SetLightEnable(false);
				scene->DebugDraw();
				std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
					if (!obj)
						return;
					if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
						obj->DebugDraw();
						ComponentWPVec comps;
						for (auto& comp : obj->components) {
							comps.push_back(comp);
						}
						std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
							if (comp)
								if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
									comp->DebugDraw();
							});
					}
					comps.clear();
					});
				scene->OnDebugDrawFinish();

				scene->LateDebugDraw();
				std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
					if (!obj)
						return;
					if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
						obj->LateDebugDraw();
						ComponentWPVec comps;
						for (auto& comp : obj->components) {
							comps.push_back(comp);
						}
						std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
							if (comp)
								if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
									comp->LateDebugDraw();
							});
					}
					comps.clear();
					});
				scene->OnLateDebugDrawFinish();
			}
		}
		SetUseLighting(true);
		SetLightEnable(true);
		Time::ResetTime();
	}
	scene->SetCurrentCamera(current_camera.lock());
	scene->PostDraw();
	std::for_each(objs.begin(), objs.end(), [&comps](ObjectWP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			try {
				obj->PostDraw();
			}
			catch (Exception& ex) {
				ex.Show();
			}
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
					{
						try {
							comp->PostDraw();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
				});
		}
		comps.clear();
		});
	scene->OnPostDrawFinish();

}


void SceneManager::Draw()
{
	{
		void* cbuffer_p = GetBufferShaderConstantBuffer(be_default_cbuffer);
		if (cbuffer_p) {
			auto* p = reinterpret_cast<Vector4*>(cbuffer_p);
			p->x = Time::SystemTime();
			p->y = Time::DrawDeltaTime();
			UpdateShaderConstantBuffer(be_default_cbuffer);
			SetShaderConstantBuffer(be_default_cbuffer, DX_SHADERTYPE_PIXEL, 4);
			SetShaderConstantBuffer(be_default_cbuffer, DX_SHADERTYPE_VERTEX, 4);
		}
	}

	for (auto& another_scene : another_scenes)
	{
		DrawCycleForOneScene(another_scene);
	}
	if (!current_scene) {
		DrawString(0, 0, ShiftJISToUTF8("カレントシーンが存在しません").c_str(), GetColor(255, 0, 0));
		return;
	}
	DrawCycleForOneScene(current_scene);
	//最後にカレントシーンのカレントカメラが持っている描画情報だけバックバッファに書き込む
	SetRenderTarget(GetBackBuffer(), GetDepthStencil());
	if (current_scene) {
		auto current_camera = current_scene->GetCurrentCamera();
#if 0
		auto debug_camera = current_scene->GetDebugCamera();
		if (debug_camera) {
			if (debug_camera->status.status_bit.is(CompStat::STATUS::ACTIVE))
				current_camera = debug_camera;
		}
#endif
		if (auto camera = current_camera.lock()) {
			ClearColor(Color::GRAY);
			int a = DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, *camera->hdr, true);
			//CopyToRenderTarget(GetBackBuffer(), camera->hdr.get());
			a++;
		}
	}
	Shader::updateFileWatcher();

}



void SceneManager::DebugDraw()
{
	return;

	//===============================//
	//デバッグ用ボックス(SooS提供)の描画
	MV1SetPosition(debug_box->handle, VGet(0, 0, 0));
	//MV1SetScale(debug_box, VGet(10, 10, 10));
	//MV1DrawModel(debug_box);

	//===============================//


	if (!current_scene) {
		DrawString(0, 0, ShiftJISToUTF8("カレントシーンが存在しません").c_str(), GetColor(255, 0, 0));
		return;
	}
	current_scene->DebugDraw();
	ObjectWPVec objs;
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [](ObjectWP& obj) {
		if (!obj)
			return;
		obj->DebugDraw();
		ComponentWPVec comps;
		for (auto& comp : obj->components) {
			comps.push_back(comp);
		}
		std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
			if (comp)
				comp->DebugDraw();
			});
		});
	current_scene->OnDebugDrawFinish();
}

void SceneManager::LateDebugDraw()
{
	return;
	if (!current_scene) {
		DrawString(0, 0, "カレントシーンが存在しません", GetColor(255, 0, 0));
		return;
	}

	current_scene->LateDebugDraw();
	ObjectWPVec objs;
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	std::for_each(objs.begin(), objs.end(), [](ObjectWP& obj) {
		if (!obj)
			return;
		obj->LateDebugDraw();
		ComponentWPVec comps;
		for (auto& comp : obj->components) {
			comps.push_back(comp);
		}
		std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
			if (comp)
				comp->LateDebugDraw();
			});
		});
	current_scene->OnLateDebugDrawFinish();
}

void SceneManager::PostDraw()
{
	if (auto ite = func_on_loop_finish.begin(); ite != func_on_loop_finish.end()) {
		if (*ite) {
			(*ite)();
			func_on_loop_finish.erase(ite);
		}
	}
	return;
	if (!current_scene)
		return;
	current_scene->PostDraw();
	ObjectWPVec objs;
	for (auto& obj : Object::GetArray<::Object>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj)
			continue;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostDraw();
			if (!obj)
				continue;
			ComponentWPVec comps;
			for (auto& comp : obj->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PostDraw();
		}
	}
	current_scene->OnPostDrawFinish();
}

void SceneManager::Exit()
{
	if (current_scene) {
		for (auto& obj : Object::GetArray<::Object>()) {

			while (auto comp = obj->GetComponent<Component>()) {
				comp->RemoveThisComponent();
			}
			obj->Exit();
			obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		}
		current_scene->Exit();
		current_scene->UnLoad();
		current_scene->Destroy();
		current_scene->DestroyPhysics();
	}
	for (auto ite = scenes.begin(); ite != scenes.end(); ite++) {
		if ((*ite) == current_scene)
		{
			scenes.erase(ite);
			current_scene = nullptr;
			break;
		}
	}

	for (auto& scene : scenes) {
		scene->UnLoad();
		scene->Destroy();
		scene->DestroyPhysics();
	}

	for (auto& another_scene : another_scenes) {
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>()) {

			while (auto comp = obj->GetComponent<Component>()) {
				comp->RemoveThisComponent();
			}
			obj->Exit();
			obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		}
		another_scene->Exit();
		another_scene->Destroy();
		another_scene->DestroyPhysics();
	}

	//===============================//
	//デバッグ用ボックス(SooS提供)の解放
	debug_box.reset();
	//===============================//
	DxLib::DeleteShaderConstantBuffer(be_default_cbuffer);
	be_default_cbuffer = -1;
	try {
		ModelManager::Exit();
	}
	catch (Exception& ex) {
		ex.Show();
	}

	try {
		TextureManager::Exit();
	}
	catch (Exception& ex) {
		ex.Show();
	}
	AudioManager::Exit();
	scenes.clear();
	another_scenes.clear();
}

void SceneManager::Object::DontDestroyOnLoad(ObjectP obj, SceneP from_where)
{
	SafeStaticCast<DontDestroyOnLoadScene>(another_scenes[0])->DontDestroyOnLoad(obj, from_where);
}


//----------------------------------------------------------------

#else

//----------------------------------------------------------------
int SceneManager::Init()
{

	TextureManager::Init();
	ModelManager::Init();
	AudioManager::Init();
	//===============================//
	//デバッグ用ボックス(SooS提供)のロード
	ModelManager::LoadAsModel("data/DebugBox/DebugMode/Box.mv1", "debug_box");
	debug_box = ModelManager::CloneModelByName("debug_box");
	//===============================//

	//裏シーンの配列の0番目は必ずここで作成するシーンが入っている
	auto default_dontdestroyonload_scene = make_safe_shared<DontDestroyOnLoadScene>();
	default_dontdestroyonload_scene->Construct();
	another_scenes[0] = default_dontdestroyonload_scene;

	return 0;
}

void SceneManager::PreUpdate()
{
	if (Input::GetKeyDown(KEY_INPUT_F4)) {
		USE_DEBUG_DRAW = !USE_DEBUG_DRAW;
	}
	//全ての裏シーンに対してPreUpdate
	for (auto& another_scene : another_scenes)
	{
		//先に、オブジェクトの優先度の同期を行う
		if (another_scene->objects.size() > 1) {
			another_scene->SyncGameObjectsPriority();

		}
		//次に、オブジェクト毎にコンポーネントの優先度の同期を行う
		for (auto& obj : another_scene->objects) {
			if (obj->components.size() > 1) {
				obj->SyncComponentsPriority();
			}
		}
		//全て終わった後にシーンのPreUpdate
		try {
			another_scene->PreUpdate();

		}
		catch (Exception& ex) {
			ex.Show();
		}

		auto& objs = another_scene->objects;
		//オブジェクト毎にPreUpdateを行う
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			//オブジェクトがnullptrなら何もしない
			if (!obj)
				return;
			//オブジェクトのステータスがアクティブならPreUpdateを行う
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->PreUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				auto& comps = obj->components;
				//コンポーネント毎にPreUpdateを行う
				std::for_each(comps.begin(), comps.end(), [&obj](ComponentP& comp) {
					//コンポーネントがnullptrなら何もしない
					if (!comp)
						return;
					//コンポーネントのステータスがアクティブならPreUpdateを行う
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
						try {
							comp->PreUpdate();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
					});
			}
			});
		//PreUpdateが終わったら、シーンのPreUpdateFinishを呼ぶ
		another_scene->OnPreUpdateFinish();
	}

	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	//カレントシーンのオブジェクトの優先度を同期
	if (current_scene->objects.size() > 1) {
		current_scene->SyncGameObjectsPriority();
	}
	//カレントシーンのオブジェクト毎にコンポーネントの優先度を同期
	for (auto& obj : current_scene->objects) {
		if (obj->components.size() > 1) {
			obj->SyncComponentsPriority();
		}
	}

	//カレントシーンのPreUpdateを呼ぶ
	current_scene->PreUpdate();


	auto& objs = current_scene->objects;

	//オブジェクト毎にPreUpdateを行う
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		//オブジェクトがnullptrなら何もしない
		if (!obj)
			return;
		//オブジェクトのステータスがアクティブならPreUpdateを行う
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			//オブジェクトのPreUpdateを行う
			obj->PreUpdate();
			auto& comps = obj->components;
			//コンポーネント毎にPreUpdateを行う
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				//コンポーネントがnullptrなら何もしない
				if (!comp)
					return;
				//コンポーネントのステータスがアクティブならPreUpdateを行う
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PreUpdate();
				});
		}
		});
	//PreUpdateが終わったら、カレントシーンのPreUpdateFinishを呼ぶ
	current_scene->OnPreUpdateFinish();
}

void SceneManager::Update()
{

	for (auto& another_scene : another_scenes)
	{
		another_scene->Update();

		auto& objs = another_scene->objects;
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {

			if (!obj)
				return;

			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->Update();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				auto& comps = obj->components;

				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
							try {
								comp->Update();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			});
		another_scene->OnUpdateFinish();
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	try {
		current_scene->Update();
	}
	catch (Exception& ex) {
		ex.Show();
	}
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->Update();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->Update();
				});
		}
		});
	current_scene->OnUpdateFinish();
}

void SceneManager::LateUpdate()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->LateUpdate();
		auto& objs = another_scene->objects;
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->LateUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						{
							try {
								comp->LateUpdate();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			});
		another_scene->OnLateUpdateFinish();
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->LateUpdate();
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->LateUpdate();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->LateUpdate();
				});
		}
		});
	current_scene->OnLateUpdateFinish();
}

void SceneManager::PostUpdate()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->PostUpdate();
		auto& objs = another_scene->objects;
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj->PostUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						{
							try {
								comp->PostUpdate();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			});
		another_scene->OnPostUpdateFinish();
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PostUpdate();
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostUpdate();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PostUpdate();
				});
		}
		});
	current_scene->OnPostUpdateFinish();
}

void SceneManager::PrePhysics()
{

	for (auto& another_scene : another_scenes)
	{
		another_scene->PrePhysics();
		auto& objs = another_scene->objects;
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->PrePhysics();
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->PrePhysics();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PrePhysics();
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PrePhysics();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PrePhysics();
				});
		}
		});
}

void SceneManager::Physics()
{
	for (auto& another_scene : another_scenes)
	{
		auto& objs = another_scene->objects;
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->Physics();
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->Physics();
					});
			}
			});
		//Physicsだけ、シーンのシミュレーションを最後に行う
		another_scene->Physics();

	}

	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->Physics();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->Physics();
				});
		}
		});

	//Physicsだけ、シーンのシミュレーションを最後に行う
	current_scene->Physics();
}

void SceneManager::PostPhysics()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->PostPhysics();
		auto& objs = another_scene->objects;
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->PostPhysics();
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->PostPhysics();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PostPhysics();
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostPhysics();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PostPhysics();
				});
		}
		});
}

//シーン一つ一つに対してPreDraw,Draw,LateDrawを別々に呼ぶとカメラの情報が競合したり、
//いろいろとナンセンスなのでやめた。
void SceneManager::DrawCycleForOneScene(SceneP scene) {
	scene->PreDraw();
	auto& objs = scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			obj->PreDraw();
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
						comp->PreDraw();
				});
		}
		});
	scene->OnPreDrawFinish();

	scene->Draw();
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			try {
				obj->Draw();
			}
			catch (Exception& ex) {
				ex.Show();
			}
			auto& comps = obj->components;

			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
					{
						try {
							comp->Draw();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
				});
		}
		});
	scene->OnDrawFinish();

	scene->LateDraw();
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			try {
				obj->LateDraw();
			}
			catch (Exception& ex) {
				ex.Show();
			}
			auto& comps = obj->components;

			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
					{
						try {
							comp->LateDraw();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
				});
		}
		});
	scene->OnLateDrawFinish();
	if (USE_DEBUG_DRAW) {
		//デバッグ描画を行う


		SetUseLighting(false);
		SetLightEnable(false);
		scene->DebugDraw();
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->DebugDraw();
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->DebugDraw();
					});
			}
			});
		scene->OnDebugDrawFinish();

		scene->LateDebugDraw();
		std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
			if (!obj)
				return;
			if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj->LateDebugDraw();
				auto& comps = obj->components;
				std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
					if (comp)
						if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp->LateDebugDraw();
					});
			}
			});
		scene->OnLateDebugDrawFinish();

		SetUseLighting(true);
		SetLightEnable(true);
	}
	scene->PostDraw();
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			try {
				obj->PostDraw();
			}
			catch (Exception& ex) {
				ex.Show();
			}
			auto& comps = obj->components;
			std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::DRAW))
					{
						try {
							comp->PostDraw();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
				});
		}
		});
	scene->OnPostDrawFinish();

}


void SceneManager::Draw()
{
	for (auto& another_scene : another_scenes)
	{
		DrawCycleForOneScene(another_scene);
	}
	if (!current_scene) {
		DrawString(0, 0, ShiftJISToUTF8("カレントシーンが存在しません").c_str(), GetColor(255, 0, 0));
		return;
	}
	DrawCycleForOneScene(current_scene);
	//最後にカレントシーンのカレントカメラが持っている描画情報だけバックバッファに書き込む
	SetRenderTarget(GetBackBuffer(), GetDepthStencil());
	if (current_scene) {
		auto current_camera = current_scene->GetCurrentCamera();
		auto debug_camera = current_scene->GetDebugCamera();
		if (debug_camera) {
			if (debug_camera->status.status_bit.is(CompStat::STATUS::ACTIVE))
				current_camera = debug_camera;
		}
		if (auto camera = current_camera.lock()) {
			ClearColor(Color::GRAY);
			int a = DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, *camera->hdr, true);
			a++;
		}
	}
	Shader::updateFileWatcher();

}



void SceneManager::DebugDraw()
{
	return;

	//===============================//
	//デバッグ用ボックス(SooS提供)の描画
	MV1SetPosition(debug_box->handle, VGet(0, 0, 0));
	//MV1SetScale(debug_box, VGet(10, 10, 10));
	//MV1DrawModel(debug_box);

	//===============================//


	if (!current_scene) {
		DrawString(0, 0, ShiftJISToUTF8("カレントシーンが存在しません").c_str(), GetColor(255, 0, 0));
		return;
	}
	current_scene->DebugDraw();
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		obj->DebugDraw();
		auto& comps = obj->components;
		std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
			if (comp)
				comp->DebugDraw();
			});
		});
	current_scene->OnDebugDrawFinish();
}

void SceneManager::LateDebugDraw()
{
	return;
	if (!current_scene) {
		DrawString(0, 0, "カレントシーンが存在しません", GetColor(255, 0, 0));
		return;
	}

	current_scene->LateDebugDraw();
	auto& objs = current_scene->objects;
	std::for_each(objs.begin(), objs.end(), [](ObjectP& obj) {
		if (!obj)
			return;
		obj->LateDebugDraw();
		auto& comps = obj->components;
		std::for_each(comps.begin(), comps.end(), [](ComponentP& comp) {
			if (comp)
				comp->LateDebugDraw();
			});
		});
	current_scene->OnLateDebugDrawFinish();
}

void SceneManager::PostDraw()
{
	if (auto ite = func_on_loop_finish.begin(); ite != func_on_loop_finish.end()) {
		if (*ite) {
			(*ite)();
			func_on_loop_finish.erase(ite);
		}
	}
	return;
	if (!current_scene)
		return;
	current_scene->PostDraw();
	auto& objs = current_scene->objects;
	for (auto& obj : objs) {
		if (!obj)
			continue;
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostDraw();
			if (!obj)
				continue;
			auto& comps = obj->components;
			for (auto& comp : comps)
				if (comp)
					if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp->PostDraw();
		}
	}
	current_scene->OnPostDrawFinish();
}

void SceneManager::Exit()
{
	if (current_scene) {
		for (auto& obj : Object::GetArray<::Object>()) {

			while (auto comp = obj->GetComponent<Component>()) {
				comp->RemoveThisComponent();
			}
			obj->Exit();
			obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		}
		current_scene->Exit();
		current_scene->UnLoad();
		current_scene->Destroy();
		current_scene->DestroyPhysics();
	}
	for (auto ite = scenes.begin(); ite != scenes.end(); ite++) {
		if ((*ite) == current_scene)
		{
			scenes.erase(ite);
			current_scene = nullptr;
			break;
		}
	}

	for (auto& scene : scenes) {
		scene->UnLoad();
		scene->Destroy();
		scene->DestroyPhysics();
	}

	for (auto& another_scene : another_scenes) {
		for (auto& obj : another_scene->GetGameObjectPtrVec<::Object>()) {

			while (auto comp = obj->GetComponent<Component>()) {
				comp->RemoveThisComponent();
			}
			obj->Exit();
			obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		}
		another_scene->Exit();
		another_scene->Destroy();
		another_scene->DestroyPhysics();
	}

	//===============================//
	//デバッグ用ボックス(SooS提供)の解放
	debug_box.reset();
	//===============================//

	try {
		ModelManager::Exit();
	}
	catch (Exception& ex) {
		ex.Show();
	}

	try {
		TextureManager::Exit();
	}
	catch (Exception& ex) {
		ex.Show();
	}
	AudioManager::Exit();
	scenes.clear();
	another_scenes.clear();
}

void SceneManager::Object::DontDestroyOnLoad(ObjectP obj, SceneP from_where)
{
	SafeStaticCast<DontDestroyOnLoadScene>(another_scenes[0])->DontDestroyOnLoad(obj, from_where);
}

#endif
//----------------------------------------------------------------
