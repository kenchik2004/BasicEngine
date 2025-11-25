#include "DiceD8.h"

namespace Sample {

	DiceD8::DiceD8()
	{
		model_name = "D8";
		dice_vectors = {

	Vector3(0.577f,-0.577f,0.577f),
	Vector3(0.577f,0.577f,0.577f),
	Vector3(-0.577f,-0.577f,-0.577f),
	Vector3(-0.577f,-0.577f,0.577f),
	Vector3(0.577f,0.577f,-0.577f),
	Vector3(0.577f,-0.577f,-0.577f),
	Vector3(-0.577f,0.577f,0.577f),
	Vector3(-0.577f,0.577f,-0.577f),

		};
	}

}
