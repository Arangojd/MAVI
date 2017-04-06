#include <cmath>

#include "mavi-math.hpp"
#include "mavi-calib.hpp"

double maviGetSlope(double m_measured, double l_measured)
{
	double m_height, l_height, m_length, l_length, net_height, net_length, measured_slope;

	m_height = m_measured * cos(MAVI_REF_ANGLE_IRM);
	l_height = l_measured * cos(MAVI_REF_ANGLE_IRL);
	net_height = m_height - l_height;

	m_length = m_measured * sin(MAVI_REF_ANGLE_IRM);
	l_length = l_measured * sin(MAVI_REF_ANGLE_IRL);
	net_length = l_length - m_length;

	measured_slope = net_height/net_length;

	return measured_slope;
}

double maviGetRefAngle(double dist_measured, double sensor_angle)
{
	double m_height, m_length, ref_angle;

	m_height = (dist_measured * cos(sensor_angle)) - MAVI_REF_DIST_IRS;
	m_length = dist_measured * sin(sensor_angle);

	ref_angle = atan(m_height/m_length) + sensor_angle;

	return ref_angle;
}
