#pragma once

#include <string>

#include "../Math/Math.h"

#define DEVELOPER_BUILD

using byte = unsigned char;

struct Color_t { byte r = 0, g = 0, b = 0, a = 0; };

struct Gradient_t {
	Color_t startColour = { 0,0,0,255 };
	Color_t endColour = { 0,0,0,255 };
};
// cry
struct Chams_t {
	bool	showObstructed = false;
	int		drawMaterial = 0;
	int		overlayType = 0;
	bool	chamsActive = false;
	Color_t overlayColor = { 0,0,0,255 };
	Color_t fresnelBaseColor = { 0,0,0,255 };
	std::string customMaterial = "None";
};

namespace Color
{
	inline float TOFLOAT(byte x) {
		return (static_cast<float>(x) / 255.0f);
	}

	inline Vec3 TOVEC3(const Color_t &x) {
		return Vec3(TOFLOAT(x.r), TOFLOAT(x.g), TOFLOAT(x.b));
	}

	inline unsigned long TODWORD(const Color_t &x) {
		return (static_cast<unsigned long>(((x.r & 0xFF) << 24) | ((x.g & 0xFF) << 16) | ((x.b & 0xFF) << 8) | (x.a & 0xFF)));
	}
}