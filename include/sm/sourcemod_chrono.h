#pragma once

#include <chrono>

namespace sm::chrono
{
	using namespace std::chrono_literals;

	struct EngineClock
	{
		using rep = float;
		using period = std::ratio<1>;
		using duration = std::chrono::duration<rep, period>;
		using time_point = std::chrono::time_point<EngineClock>;
		static constexpr bool is_steady = false;
		static time_point now() noexcept;
	};

	static_assert(sizeof(EngineClock::duration) == sizeof(float) && sizeof(EngineClock::time_point) == sizeof(float), "EngineClock has not the same layout with engine.");
	using time_point_t = EngineClock::time_point;
	using duration_t = EngineClock::duration;

	constexpr EngineClock::duration zero_duration = EngineClock::duration::zero();
	constexpr EngineClock::duration one_duration = 1.0s;
	constexpr EngineClock::time_point invalid_time_point = EngineClock::time_point(zero_duration);

	inline time_point_t EngineClock::now() noexcept
	{
		return time_point(duration_t(GetGameTime()));
	}
}