#pragma once
#include <cmath>
#include <algorithm>
#include <iostream>

class Vec3
{
public:
    double x, y, z;

    // Constructor declaration
    Vec3(double x, double y, double z) : x(x), y(y), z(z){};

    // Operator overloading declarations
    Vec3 operator+(const Vec3 &rhs) const
    {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3 operator-(const Vec3 &rhs) const
    {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vec3 operator-() const
    {
        return Vec3(-this->x, -this->y, -this->z);
    }

    double dot(const Vec3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    double mag() const
    {
        return sqrt(this->dot(*this));
    }

    Vec3 normalize() const
    {
        return *this * (1 / this->mag());
    }

    Vec3 clamp(double min_val, double max_val) const
    {
        return {
            std::min(max_val, std::max(min_val, this->x)),
            std::min(max_val, std::max(min_val, this->y)),
            std::min(max_val, std::max(min_val, this->z))};
    }

    void print() const
    {
        std::cout << "(" << this->x << ", " << this->y << ", " << this->z << ")" << std::endl;
    }

    friend Vec3 operator*(const Vec3 &vec, double scalar);
    friend Vec3 operator*(double scalar, const Vec3 &vec);
};

Vec3 operator*(const Vec3 &vec, double scalar)
{
    return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

Vec3 operator*(double scalar, const Vec3 &vec)
{
    return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}