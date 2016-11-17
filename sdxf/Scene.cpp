#include "Scene.h"
#include "Cube.h"


Scene::Scene()
{}

void Scene::AddObject(Cube c) {
	content.push_back(c);
}

Cube Scene::GetObjectAt(int index)
{
	return content[index];
}

void Scene::RemoveObject(Cube c) {
	// TODO
}

Cube Scene::GetLastObject() {
	return content.back();
}

int Scene::GetSize()
{
	return content.size();
}

XMMATRIX Scene::GetCameraPosition() {
	return cameraPosition;
}

void Scene::SetCameraPosition(XMMATRIX aCameraPosition) {
	cameraPosition = aCameraPosition;
}