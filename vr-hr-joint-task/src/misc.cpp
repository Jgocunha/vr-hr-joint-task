#include "misc.h"


double calculateEuclideanDistance(const Position& a, const Position& b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + std::pow( a.z - b.z, 2));
}

double calculateVelocity(const Position& a, const Position& b, double time)
{
	const double deltaX = a.x - b.x;
	const double deltaY = a.y - b.y;
	const double deltaZ = a.z - b.z;

	const double velocity = sqrt(pow(deltaX, 2) + pow(deltaY, 2) + pow(deltaZ, 2)) / time;
	return velocity;
}

// https://github.com/Jgocunha/action-likelihood
double calculateLikelihoodOfHumanAction(const Position& handPos, const Position& handPosPrev, const Position& componentPos, double deltaTime, double tau, double sigma)
{
	const double distance = calculateEuclideanDistance(handPos, componentPos);
	const double velocity = calculateVelocity(handPos, handPosPrev, deltaTime);

	const double exponent = -pow((distance + tau * velocity), 2) / (2 * pow(sigma, 2));
	const double likelihood = (1 / sqrt(2 * std::numbers::pi * pow(sigma, 2))) * exp(exponent);

	return likelihood;
}


