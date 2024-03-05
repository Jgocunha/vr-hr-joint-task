#pragma once

#include "coppeliasim_handler.h"
#include "dnfcomposer_handler.h"

class Experiment
{
private:
	CoppeliasimHandler coppeliasimHandler;
	DNFComposerHandler dnfcomposerHandler;
	int commsFrequency;
public:
	Experiment(std::string name, int commsFreq, double deltaT);
	~Experiment();

	void init();
	void run();
	void close();
private:
	void main();
};
