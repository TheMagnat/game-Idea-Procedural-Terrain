// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]

#include "pch.h"

double rand_noise(int t){
	t = (t << 13) ^ t;
	t = (t * (t * t * 15731 + 789221) + 1376312589);
	return 1.0 - (t & 0x7fffffff) / 1073741824.0;
}
