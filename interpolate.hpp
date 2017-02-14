/*
 * interpolate.hpp
 * ---------------
 *
 * Function declarations and inline function
 * definitions for various data interpolation methods.
 */

#ifndef INTERPOLATE_HPP
#define INTERPOLATE_HPP

inline double lerp(double x, double xmin, double xmax, double ymin, double ymax)
{
	return ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin);
}

#endif
