#pragma once
#include "Object.h"

class M_Rock : public Object 
{
private:

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Dispose() override;
};