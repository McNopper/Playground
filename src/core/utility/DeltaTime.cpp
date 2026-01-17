#include "DeltaTime.h"

DeltaTime::DeltaTime()
{
	reset();
}

void DeltaTime::reset()
{
	m_last_time = std::chrono::steady_clock::now();

	m_current_time = m_last_time;
}

double DeltaTime::tick()
{
	m_last_time = m_current_time;

	m_current_time = std::chrono::steady_clock::now();

	return (double)(m_current_time - m_last_time).count() / 1000000000.0;
}
