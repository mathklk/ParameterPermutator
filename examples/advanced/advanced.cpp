/* Requires C++17
 */

#include <chrono>
#include <iostream>
#include <csignal>
using std::cout;
using std::endl;
using std::flush;

#include "../../src/parameter_permutator.hpp"
#include "../../src/progressbar.hpp"
#include "../../src/linspace.hpp"

/* Identical to basic example
 */
float myTrainFunc(bool b, int i, float f, double d) {
	usleep(100E3);
	if (b) {
		return i * f * d;
	} else {
		return i + f + d;
	}
}
// Highly recommended: use a typedef for the specific ParameterPermutator template
typedef ParameterPermutator<float, bool, int, float, double> MyParameterPermutator;
// Make it global so we can access it from the signal handler
MyParameterPermutator* pp = nullptr;

/**
 * Advanced example
 */
int main(void) {
	// The overloaded constructor allows you to specify the parameter names through initializer lists
	// We're also using the linspace functions from linspace.hpp to generate the parameter ranges
    pp = new MyParameterPermutator(
        myTrainFunc, 
        {
            {"b", {true, false}},
            {"i", linspace_n<int>(0, 10, 10)},
            {"f", linspace_step<float>(0.0, 1.0, .1)},
            {"d", {0.5, 2.0}}
        }
    );

	// Generate a report before running. Very useful if the program takes a long time to run
	cout << pp->getReport() << "\n";
	float constexpr iterationsPerSecond = 10; // determined in previous runs
	cout << "\nEstimated time to run: " << pp->getNumberOfTotalPermutations() / iterationsPerSecond / 60 << " minutes\n";
	cout << "\nProceed? y/n: " << flush;
	char const c = getchar();
	if (c != 'y') {
		return 0;
	}

	// Add a progressbar - The generic progressbar included in this project works fine for most cases
	Progressbar<MyParameterPermutator> progressbar;
	pp->setProgressCallback([&](MyParameterPermutator* pp) {
		progressbar.callback(pp);
	});	

	// Attach sigint handler to print best parameters before exiting
	signal(SIGINT, [](int signal) {
		cout << "\n[Printing best parameters before exit]";
		cout << "\nbest score: " << pp->getBestScore();
		cout << "\nbest parameters: " << pp->to_string(pp->getBestParameters()) << endl;
		exit(signal);
	});

	pp->run();
	cout << "\nbest score: " << pp->getBestScore() << endl;
	cout << "best parameters: " << pp->to_string(pp->getBestParameters()) << endl;
	
	return 0;
}