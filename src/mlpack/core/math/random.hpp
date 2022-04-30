/**
 * @file core/math/random.hpp
 *
 * Miscellaneous math random-related routines.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_MATH_RANDOM_HPP
#define MLPACK_CORE_MATH_RANDOM_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/mlpack_export.hpp>
#include <random>

namespace mlpack {
namespace math /** Miscellaneous math routines. */ {

/**
 * MLPACK_EXPORT is required for global variables; it exports the symbols
 * correctly on Windows.
 */

#ifndef MLPACK_CORE_MATH_RANDOM_GLOBAL
#define MLPACK_CORE_MATH_RANDOM_GLOBAL

#if __cplusplus < 201703L
  namespace rand_mlpack {
    template<class=void>
    struct GlobalRandomVariables
    {
      static std::mt19937 randGen;
      static std::uniform_real_distribution<> randUniformDist;
      static std::normal_distribution<> randNormalDist;
    };
    template<>
    std::mt19937 rand_mlpack::GlobalRandomVariables<>::randGen;
    template<>
    std::uniform_real_distribution<> GlobalRandomVariables<>::randUniformDist(0.0, 1.0);
    template<>
    std::normal_distribution<> GlobalRandomVariables<>::randNormalDist(0.0, 1.0);
  }

  // Global random object.
  static MLPACK_EXPORT std::mt19937& randGen = rand_mlpack::GlobalRandomVariables<>::randGen;
  // Global uniform distribution.
  static MLPACK_EXPORT std::uniform_real_distribution<>& randUniformDist = rand_mlpack::GlobalRandomVariables<>::randUniformDist;
  // Global normal distribution.
  static MLPACK_EXPORT std::normal_distribution<>& randNormalDist = rand_mlpack::GlobalRandomVariables<>::randNormalDist;
#else
  // Global random object.
  inline std::mt19937 randGen;
  // Global uniform distribution.
  inline std::uniform_real_distribution<> randUniformDist(0.0, 1.0);
  // Global normal distribution.
  inline std::normal_distribution<> randNormalDist(0.0, 1.0);
#endif

#endif

/**
 * Set the random seed used by the random functions (Random() and RandInt()).
 * The seed is casted to a 32-bit integer before being given to the random
 * number generator, but a size_t is taken as a parameter for API consistency.
 *
 * @param seed Seed for the random number generator.
 */
inline void RandomSeed(const size_t seed)
{
  #if (!defined(BINDING_TYPE) || BINDING_TYPE != BINDING_TYPE_TEST)
    randGen.seed((uint32_t) seed);
    #if (BINDING_TYPE == BINDING_TYPE_R)
      // To suppress Found 'srand', possibly from 'srand' (C).
      (void) seed;
    #else
      srand((unsigned int) seed);
    #endif
    arma::arma_rng::set_seed(seed);
  #else
    (void) seed;
  #endif
}

/**
 * Set the random seed to a fixed number.
 * This function is used in binding tests to set a fixed random seed before
 * calling mlpack(). In this way we can test whether a certain parameter makes
 * a difference to execution of CLI binding.
 * Refer to pull request #1306 for discussion on this function.
 */
#if (BINDING_TYPE == BINDING_TYPE_TEST)
inline void FixedRandomSeed()
{
  const static size_t seed = rand();
  randGen.seed((uint32_t) seed);
  srand((unsigned int) seed);
  arma::arma_rng::set_seed(seed);
}

inline void CustomRandomSeed(const size_t seed)
{
  randGen.seed((uint32_t) seed);
  srand((unsigned int) seed);
  arma::arma_rng::set_seed(seed);
}
#endif

/**
 * Generates a uniform random number between 0 and 1.
 */
inline double Random()
{
  return randUniformDist(randGen);
}

/**
 * Generates a uniform random number in the specified range.
 */
inline double Random(const double lo, const double hi)
{
  return lo + (hi - lo) * randUniformDist(randGen);
}

/**
 * Generates a 0/1 specified by the input.
 */
inline double RandBernoulli(const double input)
{
  if (Random() < input)
    return 1;
  else
    return 0;
}

/**
 * Generates a uniform random integer.
 */
inline int RandInt(const int hiExclusive)
{
  return (int) std::floor((double) hiExclusive * randUniformDist(randGen));
}

/**
 * Generates a uniform random integer.
 */
inline int RandInt(const int lo, const int hiExclusive)
{
  return lo + (int) std::floor((double) (hiExclusive - lo)
                               * randUniformDist(randGen));
}

/**
 * Generates a normally distributed random number with mean 0 and variance 1.
 */
inline double RandNormal()
{
  return randNormalDist(randGen);
}

/**
 * Generates a normally distributed random number with specified mean and
 * variance.
 *
 * @param mean Mean of distribution.
 * @param variance Variance of distribution.
 */
inline double RandNormal(const double mean, const double variance)
{
  return variance * randNormalDist(randGen) + mean;
}

/**
 * Obtains no more than maxNumSamples distinct samples. Each sample belongs to
 * [loInclusive, hiExclusive).
 *
 * @param loInclusive The lower bound (inclusive).
 * @param hiExclusive The high bound (exclusive).
 * @param maxNumSamples The maximum number of samples to obtain.
 * @param distinctSamples The samples that will be obtained.
 */
inline void ObtainDistinctSamples(const size_t loInclusive,
                                  const size_t hiExclusive,
                                  const size_t maxNumSamples,
                                  arma::uvec& distinctSamples)
{
  const size_t samplesRangeSize = hiExclusive - loInclusive;

  if (samplesRangeSize > maxNumSamples)
  {
    arma::Col<size_t> samples;

    samples.zeros(samplesRangeSize);

    for (size_t i = 0; i < maxNumSamples; ++i)
      samples [ (size_t) math::RandInt(samplesRangeSize) ]++;

    distinctSamples = arma::find(samples > 0);

    if (loInclusive > 0)
      distinctSamples += loInclusive;
  }
  else
  {
    distinctSamples.set_size(samplesRangeSize);
    for (size_t i = 0; i < samplesRangeSize; ++i)
      distinctSamples[i] = loInclusive + i;
  }
}

} // namespace math
} // namespace mlpack

#endif // MLPACK_CORE_MATH_MATH_LIB_HPP
