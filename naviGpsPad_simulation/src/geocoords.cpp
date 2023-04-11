#include <math.h>
#include "geocoords.h"



/*
class GeoCoordinate
{
};*/

double getBearing(GeoCoordinate startCoord, GeoCoordinate endCoord)
{
    double y = sin(endCoord.longitude - startCoord.longitude) * cos(endCoord.latitude);
    double x = cos(startCoord.latitude) * sin(endCoord.latitude) - sin(startCoord.latitude) * cos(endCoord.latitude) * cos(endCoord.longitude - startCoord.longitude);
    double bearingRadians = atan2(y, x);
    double bearingDegrees = bearingRadians * (180 / PI);
    bearingDegrees = fmod(bearingDegrees + 360, 360);

    return bearingDegrees;
}

double getDistance(GeoCoordinate startCoord, GeoCoordinate endCoord)
{
    double distance = acos(sin(startCoord.latitude) * sin(endCoord.latitude) + cos(startCoord.latitude) * cos(endCoord.latitude) * cos(endCoord.longitude - startCoord.longitude) ) * 6371;

    return distance;
}
//dd.dddd
double getradDistance(GeoCoordinate startCoord, GeoCoordinate endCoord)
{
  double  t1 = sin(startCoord.latitude) * sin(endCoord.latitude);
  double  t2 = cos(startCoord.latitude) * cos(endCoord.latitude);
  double t3 = cos(startCoord.longitude- endCoord.longitude);
  double t4 = t2 * t3;
  double t5 = t1 + t4;
  double rad_dist = atan( - t5 / sqrt( - t5 * t5 + 1)) + 2 * atan(1);
  return (rad_dist);
}