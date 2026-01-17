#ifndef CORE_UTILITY_DELTATIME_H_
#define CORE_UTILITY_DELTATIME_H_

#include <chrono>

class DeltaTime
{

private:

	std::chrono::time_point<std::chrono::steady_clock> m_last_time{};
	std::chrono::time_point<std::chrono::steady_clock> m_current_time{};

public:

	DeltaTime();

	DeltaTime(const DeltaTime&) = delete;
	DeltaTime(DeltaTime&&) = delete;

	DeltaTime operator=(const DeltaTime&) = delete;
	DeltaTime operator=(DeltaTime&&) = delete;

	void reset();

	double tick();

};

#endif /* CORE_UTILITY_DELTATIME_H_ */
