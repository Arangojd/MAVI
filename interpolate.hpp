/*
 * interpolate.hpp
 * ---------------
 *
 * Function declarations and inline function
 * definitions for various data interpolation methods.
 */

#ifndef INTERPOLATE_HPP
#define INTERPOLATE_HPP

/**
 * Performs linear interpolation within the range [xmin, xmax] * [ymin, ymax].
 *
 * @param x Interpolant
 * @param xmin x-dimension lower bound
 * @param xmax x-dimension upper bound
 * @param ymin y-dimension lower bound
 * @param ymax y-dimension upper bound
 *
 * @returns the result of the interpolation operation.
 */
inline double lerp(double x, double xmin, double xmax, double ymin, double ymax)
{
	return ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin);
}

#endif
