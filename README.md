# ParameterPermutator

ParameterPermutator is a simple tool for finding the optimal parameters for an arbitrary black-box function.
This is done by trying out all possible combinations of the parameters within a given range.

If you're looking for an elegant solution, keep searching.
This tool is not elegant, it's a sledgehammer.

# Why?

Trying out all possible combinations is highly unoptimized and takes forever.
However, there are cases where runtime is not a priority.
You can integrate this tool into an existing C++ project within a few minutes and then let it grind away.

# Features

- Header-only library for easy integration
- Unlimited amount of parameters
- Unlimited amount of parameter values
- All base types supported

# Requirements

ParameterPermutator is written in pure C++ and does not have dependencies besides the standard library.
C++17 is required.

# Example

```cpp
/* This is the function that you want to find the best parameters for.
 * It may take an arbitrary amount of parameters and must returns a numeric 'score'
 *
 * ParameterPermutator will find the parameters where the score is the highest
 */
float myTrainFunc(bool b, int i, float f, double d) {
	...
}

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

	// Try out all permutations
	pp.run();

	cout << "best score: " << pp.getBestScore() << endl;
	cout << "best parameters: " << pp.to_string(pp.getBestParameters()) << endl;

	/*
	best score: 120
	best parameters: {P0=1, P1=3, P2=20.000000, P3=2.000000, }
	*/
	
	return 0;
}

```

ParameterPermutator supports additional features not listed here, see `examples/example.cpp` for more details.