#ifndef IBL_SAMPLER_H
#define IBL_SAMPLER_H

#include "typedefs.h"
#include "Texture.h"
#include "Light.h"
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <iomanip>

struct Region {
    int x, y, width, height;
    Region(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height)
    {
    }
    // Overload equality operator
    bool operator==(const Region& other) const
    {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
    bool operator!=(const Region& other) const
    {
        return !(*this == other);
    }
};

template <typename T>
class SummedTextureAreaContainer { // This class will hold parts of the summed area table
private:
    Region region; // The region of texture spanned by this container
    std::vector<T>* summedAreaTable; // The summed area table
public:
    SummedTextureAreaContainer(Region region, Texture* texture);
    ~SummedTextureAreaContainer() { delete summedAreaTable; }
    T getArea(int x, int y)
    {
        return summedAreaTable->at((y-1) * region.width + x-1);
    }
    Region getRegion() { return region; }
    std::vector<T>* getSummedAreaTable() { return summedAreaTable; }
};

template <typename T>
class SummedTextureArea {
public:
    SummedTextureArea(Texture* texture);
    T getArea(const Region& region);
    ~SummedTextureArea();

private:
    std::vector<SummedTextureAreaContainer<T>*>* containers;
    int width, height;
    void calculateSummedAreaTable(Texture* texture);
    T getArea(int x, int y);
};

Vector3 equirectangularToCubemapProjection(const Vector2& v, int width, int height);

class IBLSampler {
public:
    IBLSampler(Texture* hdrTexture, int numLights);
    ~IBLSampler();

    void updateLighting();

    void changeNumLights(int numLights);

    std::vector<Light*>* getLights() { return &lights; }

private:
    Texture* hdrTexture;
    std::vector<Light*> lights;
    SummedTextureArea<long double>* summedTextureArea;
    int numLights;

    void calculateLights(); // Use the median cut algorithm here
    void medianCut(Region& region, int depth, std::vector<Region>& regions);
};

#endif

// Path: src/IBLSampler.cpp