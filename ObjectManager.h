#pragma once
#include "struct.h"
#include <fstream>
#include <sstream>

class ObjectManager
{
public:
	static vector<ObjectData> objectData;

	static void LoadObject(int dataID, Sprite& sprite, const char* fileName = 0);

	static void CreateObject(const char* fileName, Sprite& sprite);

	static void CreateCone(Sprite& sprite);

	static void CreateCylinder(Sprite& sprite);

	static void CreateSphere(Sprite& sprite);
};