#include <cmath>

double maviGetSlope(double s_measured, double m_measured, double l_measured) 
{
	double m_height, l_height, m_length, l_length, net_height, net_length, ref_slope, measured_slope, actual_slope;

	m_height = s_measured - (m_measured * cos(m_ref_angle));
	l_height = s_measured - (l_measured * cos(l_ref_angle));
	net_height = l_height - m_height;

	m_length = m_length * sin(refAngleIRM);
	l_length = l_length * sin(refAngleIRL);
	net_length = l_length - m_length;

	measured_slope = net_height/net_length;
	actual_slope = measured_slope + refSlope;
	
	return actual_slope;
}

double maviGetRefSlope(double s_measured, double m_measured, double l_measured)
{
	
}
