#include <cmath>

#include "mavi-math.hpp"
#include "mavi-calib.hpp"

double maviGetSlope(double s_measured, double m_measured, double l_measured)
{
	double m_height, l_height, m_length, l_length, net_height, net_length, measured_slope, actual_slope;

	m_height = s_measured - (m_measured * cos(MAVI_REF_ANGLE_IRM));
	l_height = s_measured - (l_measured * cos(MAVI_REF_ANGLE_IRL));
	net_height = l_height - m_height;

	m_length = m_measured * sin(MAVI_REF_ANGLE_IRM);
	l_length = l_measured * sin(MAVI_REF_ANGLE_IRL);
	net_length = l_length - m_length;

	measured_slope = net_height/net_length;
	actual_slope = measured_slope - MAVI_REF_SLOPE;

	return actual_slope;
}

double maviGetRefSlope(double s_measured, double m_measured, double l_measured)
{
	double m_height, l_height, m_length, l_length, net_height, net_length, ref_slope;

	m_height = s_measured - (m_measured * cos(MAVI_REF_ANGLE_IRM));
	l_height = s_measured - (l_measured * cos(MAVI_REF_ANGLE_IRL));
	net_height = l_height - m_height;

	m_length = m_measured * sin(MAVI_REF_ANGLE_IRM);
	l_length = l_measured * sin(MAVI_REF_ANGLE_IRL);
	net_length = l_length - m_length;

	ref_slope = net_height/net_length;

	return ref_slope;
}
