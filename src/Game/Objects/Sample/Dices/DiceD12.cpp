#include "DiceD12.h"




namespace Sample {

	DiceD12::DiceD12()
	{
		model_name = "D12";
		dice_vectors = {
		 Vector3(0, 1, 0),
	 Vector3(0.276f,  0.447f,  0.850f)	,
	 Vector3(-0.723f,0.447f,0.525f)	,
	 Vector3(-0.723f,0.447f,-0.525f)	,
	 Vector3(0.276f,  0.447f,  -0.850f)	,
	 Vector3(0.894f,0.447f,0),
	 Vector3(-0.276f,-0.447f,0.850f)	,
	 Vector3(-0.894f,-0.447f,0),
	 Vector3(0.723f,-0.447f,0.525f)	,
	 Vector3(-0.276f,-0.447f,-0.850f)	,
	 Vector3(0,-1,0),
	 Vector3(0.723f,-0.447f,-0.525f),


		};
	}
}