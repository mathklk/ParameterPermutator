#ifndef LINSPACE_HPP
#define LINSPACE_HPP

#include "parameter_permutator.hpp"

/**
 * @brief Return evenly spaced numbers of type T in a given interval.
 *        You specify the number of elements.
 * 
 * This function is intended specifically for generating ranges for the ParameterPermutator and thus returns a vector of Variants.
 */
template <typename T>
static std::vector<Variant> linspace_n(T const start, T const end, size_t const n, bool const endpoint = true)
{
	// Calculate the values as floats regardless of template
	// This is done to avoid quantization errors for ints
	std::vector<float> calc(n);
	float const range = end - start;
	float const step = (endpoint ? range / (n - 1) : range / n);
	for (size_t i = 0; i < n; ++i) {
		calc[i] = start + i * step;
	}

	// Convert to desired template type (within )
	// Filter out duplicates. This can happen for integer types.
	std::vector<Variant> ret;
	for (auto const& c : calc) {
		// check if c is already in ret
		bool found = false;
		for (auto const& r : ret) {
			if (static_cast<T>(c) == std::get<T>(r)) {
				found = true;
				continue;
			}
		}
		ret.push_back(static_cast<T>(c));
	}
	return ret;
}

/**
 * @brief Return evenly spaced numbers of type T in a given interval.
 *        You specify the step size.
 */
template <typename T>
static std::vector<Variant> linspace_step(T const start, T const end, float const step, bool const endpoint = true)
{
	std::vector<Variant> ret;
	float const range = end - start;
	size_t const n = static_cast<size_t>(range / step) + 1;
	return linspace_n(start, end, n, endpoint);
}

#endif // LINSPACE_HPP
