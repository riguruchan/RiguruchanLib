#include "MathUtility.h"
#include <algorithm>
#include <cmath>

#undef min
#undef max

const float Math::PI = 3.1415926535897932f;

float Math::sin(float angle){
	return std::sin(radian(angle));
}
float Math::cos(float angle){
	return std::cos(radian(angle));
}
float Math::tan(float angle){
	return std::tan(radian(angle));
}

float Math::asin(float x){
	return angle(std::asin(x));
}
float Math::acos(float x){
	return angle(std::acos(x));
}
float Math::atan(float y, float x){
	return angle(std::atan2(y, x));
}

float Math::radian(float angle){
	return angle * (PI / 180.0f);
}
float Math::angle(float radian){
	return radian * (180.0f / PI);
}

float Math::clamp(float target, float mn, float mx){
	return std::min(mx, std::max(mn, target));
}
float Math::lerp(float s, float e, float t){
	return ((s * (1.0f - t)) + (e * t));
}
float Math::random(float _min, float _max){
	return ((float)std::rand() / RAND_MAX) * ((_max)-(_min)) + (_min);
}