#include <cmath>

#include "mavi-math.hpp"
#include "mavi-calib.hpp"

double maviGetSlope(double m_measured, double l_measured)
{
	double measured_slope, relativeDif_IRM, relativeDif_IRL;

	if (l_measured < m_measured || m_measured < MAVI_MIN_DIST_SLOPE || l_measured < MAVI_MIN_DIST_SLOPE)
		return MAVI_SLOPE_INF;

	relativeDif_IRM = MAVI_REF_DIST_IRM - m_measured;
	relativeDif_IRL = MAVI_REF_DIST_IRL - l_measured;

	measured_slope = (relativeDif_IRL + relativeDif_IRM)/(MAVI_REF_DIST_IRL + MAVI_REF_DIST_IRM);

	return measured_slope;
}
