#include "trilaterate.h"
#include "debug.h"

void locatePosition(const std::vector<Circle>&, Point *, int *);

void setupPermissibleArea(float, float, float, float);

bool generateWAPPosition(const std::vector<float>&, const std::vector<float>&, const std::vector<int>&, const std::string&);

bool findCurrentPosition(const std::vector<std::string>&, const std::vector<int32_t>&, float *, float *);
