#pragma once

struct Position
{
	double x, y, z;

	Position() = default;
	Position(double x, double y, double z)
		: x(x), y(y), z(z)
	{}
};