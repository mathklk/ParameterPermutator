#ifndef TRAINING_HPP
#define TRAINING_HPP

/* Requires C++17 */

#include <functional>
#include <variant>
#include <vector>
#include <utility>
#include <string>

#include <iostream>
using namespace std;

typedef std::variant<bool, unsigned int, int, float, double> Variant;
typedef std::vector<Variant> ParameterPack;
typedef std::vector<Variant> ParameterRange;

template <typename RetType, typename... Args>
class ParameterPermutator
{
typedef std::function<RetType(Args...)> TrainSig;
typedef std::function<void(ParameterPermutator*)> ProgressCallbackSig;

public:
	ParameterPermutator(TrainSig const userFunction, std::vector<ParameterRange> const& parameterRange, std::vector<std::string> const& parameterNames = {}):
		_userFunction(userFunction),
		_parameterRanges(parameterRange),
		_parameterNames(parameterNames)
	{
		if (sizeof...(Args) != parameterRange.size()) {
			throw std::runtime_error("Number of arguments does not match the size of the vector (nArgs=" + std::to_string(sizeof...(Args)) + ", ranges.size()=" + std::to_string(parameterRange.size()) + ")");
		}
		if (parameterNames.size() != 0 && parameterNames.size() != sizeof...(Args)) {
			throw std::runtime_error("Number of parameter names does not match number of arguments");
		}
	}
	ParameterPermutator(TrainSig const userFunction, std::vector<std::pair<std::string, ParameterRange>> nameRangeMap):
		_userFunction(userFunction)
	{
		if (sizeof...(Args) != nameRangeMap.size()) {
			throw std::runtime_error("Number of arguments does not match the size of the vector (nArgs=" + std::to_string(sizeof...(Args)) + ", vector.size()=" + std::to_string(nameRangeMap.size()) + ")");
		}
		for (auto const& [name, range] : nameRangeMap) {
			_parameterNames.push_back(name);
			_parameterRanges.push_back(range);
		}
	}

	/**
	 * @brief Get the number of total permutations that will be tried out.
	 *        It's a good idea to check this number and use to estimate a run time beofre calling @ref run().
	 * 
	 * @return size_t 
	*/
	size_t getNumberOfTotalPermutations(void) const {
		size_t N = 1;
		for (auto const& v : _parameterRanges) {
			N *= v.size();
		}
		return N;
	}
	size_t getCurrentIteration(void) const {
		return _prog;
	}
	ParameterPack getBestParameters(void) const {
		return _bestParameters;
	}
	RetType getBestScore(void) const {
		return _bestScore;
	}

	/**
	 * @brief Set an optional callback function that is called after each iteration. 
	 *        The callback function takes one argument: a pointer to the ParameterPermutator instance.
	 * 
	 * @warning The callback is blocking. It is called from the same thread that called @ref run().
	 *          For optimal performance, the callback function should return as fast as possible.
	 */
	void setProgressCallback(ProgressCallbackSig const& progressCallback) {
		_progressCallback = progressCallback;
	}


	std::string getReport(void) const {
		std::string s = "[ParameterPermutator report]";
		for (size_t i = 0; i < sizeof...(Args); ++i) {
			s += "\n" + _getParameterName(i) + ": n=" + std::to_string(_parameterRanges[i].size());
		}
		s += "\n-----------------------------";
		s += "\nTotal number of permutations: " + std::to_string(getNumberOfTotalPermutations());
		return s;
	}

	/**
	 * @brief Try out all specified permutations. The best score and the corresponding parameters are stored.
	 * 
	 * @see getBestParameters
	 * @see getBestScore
	*/
	void run(void) {
		std::vector<size_t> indiciesHandover(sizeof...(Args), 0);
		_runRecursive(indiciesHandover, 0);
	}

	/**
	 * @brief Convert a ParameterPack to a string
	*/
	std::string to_string(ParameterPack const& pp) const {
		std::string ret = "{";
		constexpr auto size = sizeof...(Args);
		for (size_t i = 0; i < size; ++i) {
			ret += _getParameterName(i) + "=";
			std::visit([&ret](auto&& arg) {
				ret += std::to_string(arg) + ", ";
			}, pp[i]);
		}
		ret += "}";
		return ret;
	}

private:
	/**
	 * @brief Create a ParameterPack by mapping the indicies to the elements in _parameterRanges
	*/
	ParameterPack _makeParameterPack(std::vector<size_t>& indicies) {
		if (indicies.size() != sizeof...(Args)) {
			throw std::runtime_error("_makeParameterPack | Number of indicies does not match number of arguments");
		}

		ParameterPack ret;
		ret.reserve(sizeof...(Args));
		for (size_t i = 0; i < sizeof...(Args); ++i) {
			ret.push_back(_parameterRanges[i][indicies[i]]);
		}
		return ret;
	}

	// This is C++17 parameter expansion magic to call the userFunction with a ParameterPack
	RetType _call(ParameterPack const& pp) {
		return _callHelper<sizeof...(Args)>(pp);
	}
	template <size_t... Is>
	RetType _callHelper(ParameterPack const& pp, std::index_sequence<Is...> const&) {
		return _userFunction(std::get<Args>(pp[Is])...);
	}
	template <size_t N>
	RetType _callHelper(ParameterPack const& pp) {
		return _callHelper(pp, std::make_index_sequence<N>{});
	}

	void _runRecursive(std::vector<size_t>& indiciesHandover, size_t i) {
		// Base case
		// The function was called from the last depth of recursion.
		// i is beyond the last index of the vector
		if (i == sizeof...(Args) ) {
			ParameterPack const pp = _makeParameterPack(indiciesHandover);
			RetType const score = _call(pp);
			if (score > _bestScore) {
				_bestScore = score;
				_bestParameters = pp;
			}
			_prog++;
			_progressCallback(this);
			return;
		}

		// Recursive case: The function was called from a normal layer
		// Iterate over the current vector and recursively call the function again
		for (size_t curi = 0; curi < _parameterRanges[i].size(); ++curi) {
			indiciesHandover[i] = curi;
			// cout << "i: " << i << " " << to_string<size_t>(indiciesHandover) << endl;
			_runRecursive(indiciesHandover, i + 1);
		}
	}

	std::string _getParameterName(size_t const i) const {
		if (_parameterNames.size() == 0 || _parameterNames[i].size() == 0) {
			return "P" + std::to_string(i);
		}
		return _parameterNames[i];
	}

private:
	TrainSig _userFunction;
	std::vector<ParameterRange> _parameterRanges;
	std::vector<std::string> _parameterNames;
	ProgressCallbackSig _progressCallback;

	size_t _prog = 0;
	RetType _bestScore = std::numeric_limits<RetType>::lowest();
	ParameterPack _bestParameters;
};

#endif // TRAINING_HPP