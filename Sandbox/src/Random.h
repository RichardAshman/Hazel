#pragma once

#include <random>

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}
	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max(); // Gives a float between 0 and 1
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};
// rand num to test in the future
//uint f_randi(uint32_t index)
//{
//	index = (index << 13) ^ index;
//	return ((index * (index * index * 15731 + 1789221) + 1376312589) & 0x7fffffff);
//}