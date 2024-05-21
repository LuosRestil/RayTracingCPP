#include <string>
#include "Vec3.h"

struct Light
{
    std::string type;
    double intensity;
    Vec3 position;
};