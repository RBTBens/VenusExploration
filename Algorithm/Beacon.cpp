#include "Definitions.h"
#include "Beacon.h"

// Checking function
bool Beacon::seeHome()
{
  return false;
}

// Normalizes degree [moved from UDS because it is only needed with finding the lab (look at which degree the beacon is)]
//int UDS::normalizeDegree(int degree)
//{
//  return -degree + UDS_ANGLE_BASE;
//}
