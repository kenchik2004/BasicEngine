#pragma once
//システムのコンポーネント群をまとめたヘッダーファイル

//システムの根幹に関わるコンポーネント
#include "System/Components/Component.h"
#include "System/Components/Transform.h"
#include "System/Components/Camera.h"


//モデルアニメーション関連コンポーネント
#include "System/Components/ModelRenderer.h"
#include "System/Components/Animator.h"


//物理演算関連コンポーネント
#include "System/Components/RigidBody.h"
#include "System/Components/Collider.h"
#include "System/Components/SphereCollider.h"
#include "System/Components/BoxCollider.h"
#include "System/Components/CapsuleCollider.h"
#include "System/Components/MeshCollider.h"
#include "System/Components/ConvexMeshCollider.h"

//オーディオ関連コンポーネント
#include "System/Components/AudioListener.h"
#include "System/Components/AudioPlayer.h"

//2D描画関連コンポーネント
#include "System/Components/ImageRenderer.h"

