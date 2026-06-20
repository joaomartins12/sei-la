

#include "pSkill.h"
#include <math.h>


int nSkill::GetReachedTime(int nDistance, int nSpeed, int nAcceleration)
{
	if(nSpeed <= 0)
	{
		return 0;
	}

	if(nAcceleration <= 0)
	{
		return nDistance * 1000 / nSpeed;
	}
	
	float t;

	t = (float)(nSpeed * nSpeed + 2 * nAcceleration * nDistance);
	t = sqrt(t);

	t = t - nSpeed;
	t = t * 1000 / nAcceleration; // convert second into millisecond

	return (int)t;
}



