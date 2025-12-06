#pragma once

namespace NonFade {
	class Enemy;
	class AbstractEnemyBrain
	{
	public:
		virtual ~AbstractEnemyBrain() = default;
		virtual void Think() = 0;

	};
}

