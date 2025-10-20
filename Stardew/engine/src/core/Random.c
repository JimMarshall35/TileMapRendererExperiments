#include "Random.h"
#include <stdio.h>
#include <stdlib.h>
#include "AssertLib.h"

float Ra_FloatBetween(float min, float max)
{
    EASSERT(max > min); 
    float random = ((float) rand()) / (float) RAND_MAX;

    float range = max - min;  
    return (random*range) + min;
}