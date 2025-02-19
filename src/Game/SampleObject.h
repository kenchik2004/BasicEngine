#pragma once
USING_PTR(SampleObject);
class SampleObject :public Object
{
public:
	USING_SUPER(SampleObject);
	int Init() override;
	void Update() override;
	void Draw() override;
	void PreDraw() override;
	void DebugDraw()override;
};


