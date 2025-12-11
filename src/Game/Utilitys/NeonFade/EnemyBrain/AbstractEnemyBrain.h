#pragma once

namespace NeonFade {
	class EnemyStateMachine;
	USING_PTR(Player);
	class Enemy;
	class AbstractEnemyBrain
	{
	public:
		AbstractEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_);

		virtual ~AbstractEnemyBrain();
		virtual void Think() = 0;
		virtual void Damage(u32 damage = 0, bool ignore_i_frame = false) = 0;
		virtual void KnockBack(Vector3 knock_back_vec) = 0;
		virtual void DebugDraw() = 0;
		u32 GetHp() { return hp; }
		bool IsDead() { return hp == 0; }
		EnemyStateMachine* GetMachine() { return state_machine; }
		Enemy* GetOwnerBody() { return body; }
	protected:
		EnemyStateMachine* state_machine;
		PlayerWP player;
		Enemy* body;
		u32 hp = 100;
		void DrawHPDebug();
		static inline SafeSharedPtr<Texture> hp_bar = nullptr;
		static inline u32 instance_count = 0;

	};
}

