#pragma once

class Math{
public:
	static const float PI;

	static float sin(float angle);
	static float cos(float angle);
	static float tan(float angle);

	static float asin(float x);
	static float acos(float x);
	static float atan(float y, float x);

	static float radian(float angle);
	static float angle(float radian);

	static float clamp(float target, float mn, float mx);
	static float lerp(float s, float e, float t);
	static float random(float _min, float _max);
};