#pragma once

#include <vector>

#include "Cube.h"

class Scene {
public:
	Scene();
	void AddObject(Cube c);
	Cube GetObjectAt(int index);
	void RemoveObject(Cube c);
	Cube GetLastObject();
	int GetSize();
	XMMATRIX GetCameraPosition();
	void SetCameraPosition(XMMATRIX aCameraPosition);

private:
	std::vector<Cube> content;
	XMMATRIX cameraPosition;
};