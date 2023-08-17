/* Requires C++17
 */

#include <iostream>
using std::cout;
using std::endl;
using std::flush;

#include "../src/parameter_permutator.hpp"

/* This is the function that you want to find the best parameters for.
 * It may take an arbitrary amount of parameters and must returns a numeric 'score'
 *
 * ParameterPermutator will find the parameters where the score is the HIGHEST
 */
float myTrainFunc(bool b, int i, float f, double d) {
	if (b) {
		return i * f * d;
	} else {
		return i + f + d;
	}
}

/**
 * Basic example how to create a ParameterPermutator and run it
 */
int main(void) {
	//      Return type of training function
	//                    |
	//                    |  Training function parameters
	//                    |     |     |     |      |
	//                    v     v     v     v      v
	ParameterPermutator<float, bool, int, float, double> pp(
		myTrainFunc,
		{
			{true, false},   // range to try out for first parmeter
			{1,2,3},         // range to try out for second parmeter
			{10.0f, 20.0f},  // ...
			{0.5, 2.0}
		}
	);
	// Internally, the ParameterPermutator used std::variant to store the parameters.
	// This means that when using an initilizer list like here, you need to explicitly
	// specify the type of the literals.
	// (e.g. 10.0f for a float, 10.0 would be a double)

	pp.run();
	cout << "best score: " << pp.getBestScore() << endl;
	cout << "best parameters: " << pp.to_string(pp.getBestParameters()) << endl;

	/*
	best score: 120
	best parameters: {P0=1, P1=3, P2=20.000000, P3=2.000000, }
	*/
	
	return 0;
}

/**
 * Advanced example
 * Specify parameter names, a callback and see the report
 */
int main2(void) {
	typedef ParameterPermutator<float, bool, int, float, double> MyParameterPermutator;

	MyParameterPermutator pp(
		myTrainFunc, 
		{
			{"b",             {true, false}}, 
			{"int parameter", {1,2,3}},       
			{"foobar",        {10.0f, 20.0f}},
			{"",              {0.5, 2.0}}
		}
	);

	cout << pp.getReport() << endl;
	float cycleEstimation_us = 18; // determined in previous run
	cout << "Estimated total runtime: " << pp.getNumberOfTotalPermutations() * cycleEstimation_us / (60*1000*1000) << " min" << endl;

	pp.setProgressCallback([](MyParameterPermutator* p) {
		cout << "\riteration " << p->getCurrentIteration() << "/" << p->getNumberOfTotalPermutations() << " | best score: " << p->getBestScore() << flush;
	});

	pp.run();
	cout << "\nbest score: " << pp.getBestScore() << endl;
	cout << "best parameters: " << pp.to_string(pp.getBestParameters()) << endl;
	
	return 0;
}