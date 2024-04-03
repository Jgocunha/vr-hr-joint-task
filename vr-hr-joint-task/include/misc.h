#pragma once

#include <cmath>
#include <numbers>
#include <chrono>

struct Position
{
	double x, y, z;

	Position() = default;
	Position(double x, double y, double z)
		: x(x), y(y), z(z)
	{}
};

struct Orientation
{
	double alpha, beta, gamma;

	Orientation(double alpha, double beta, double gamma) : alpha(alpha), beta(beta), gamma(gamma) {}
	Orientation()
		: alpha(0), beta(0), gamma(0)
	{}
};

struct Pose
{
	Position position;
	Orientation orientation;

	Pose() = default;
	Pose(const Position& position, const Orientation& orientation)
		: position(position), orientation(orientation)
	{}
};

double calculateEuclideanDistance(const Position& a, const Position& b);

double calculateVelocity(const Position& a, const Position& b, double time);

double calculateLikelihoodOfHumanAction(const Position& handPos, const Position& handPosPrev, const Position& componentPos, double deltaTime, double tau, double sigma);