#pragma once

#include <iostream>
#include <SFML\Graphics.hpp>
#include <windows.h>

using namespace std;
using namespace sf;

String getAppPath();

String getAssetsPath();

float ParticleSize(int size);

int Superior(int a, int b);
int Inferior(int a, int b);
int Sign(float a, float b);
