#include "Cube.h"

Cube::Cube(XMFLOAT3 aLocation, XMFLOAT3 aScale, XMFLOAT3 aRotation) {
	location = aLocation;
	scale = aScale;
	rotation = aRotation;
	XMMATRIX translationMatrix = XMMatrixTranslation(location.x, location.y, location.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotationMatrix = XMMatrixIdentity(); // TODO
	transform = translationMatrix * scaleMatrix * rotationMatrix;
}

XMMATRIX Cube::GetTransform()
{
	return transform;
}