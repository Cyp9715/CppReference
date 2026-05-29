#include "cyp_timer.hpp"

namespace cyp
{
	namespace timer
	{
#if _HAS_CXX20
		template long long Timer::getTime<std::chrono::months>();
		template long long Timer::getTime<std::chrono::years>();
		template long long Timer::getTime<std::chrono::weeks>();
		template long long Timer::getTime<std::chrono::days>();
#endif
		template long long Timer::getTime<std::chrono::hours>();
		template long long Timer::getTime<std::chrono::minutes>();
		template long long Timer::getTime<std::chrono::seconds>();
		template long long Timer::getTime<std::chrono::milliseconds>();
		template long long Timer::getTime<std::chrono::microseconds>();
		template long long Timer::getTime<std::chrono::nanoseconds>();

		void Timer::Start()
		{
			_start = std::chrono::system_clock::now();
		}

		template<typename T>
		long long Timer::getTime()
		{
			_current = std::chrono::system_clock::now();
			T t = std::chrono::duration_cast<T>(_current - _start);
			return t.count();
		}

		int FpsCounter::LoopCounting()
		{
			++_tempFps;
			if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _loopStart) >= std::chrono::seconds(1))
			{
				_loopFps = _tempFps;
				_tempFps = 0;
				_loopStart = std::chrono::system_clock::now();
				return _loopFps;
			}

			return _loopFps;
		}


		void FpsCounter::StartTimeCounting()
		{
			_start = std::chrono::system_clock::now();
		}

		float FpsCounter::EndTimeCounting()
		{
			_end = std::chrono::system_clock::now();
			std::chrono::milliseconds delta = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);

			if (delta.count() > 0)
				return MILISECOND / delta.count();

			return 0;
		}

		void CallbackTimer::Regist(int miliseconds, std::function<void()>& func)
		{
			std::thread([miliseconds, func]() -> void
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
					func();
				}).detach();
		}

		void CallbackTimer::Regist_loop(int &miliseconds, std::function<void()>& func, bool& isLoop)
		{
			std::thread([miliseconds, func, &isLoop]() -> void
				{
					do
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
						func();
					} while (isLoop == true);
				}).detach();
		}

	}
}
