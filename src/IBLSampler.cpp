#include "IBLSampler.h"

IBLSampler::IBLSampler(Texture* hdrTexture, int numLights)
    : hdrTexture(hdrTexture), numLights(numLights)
{
    summedTextureArea = new SummedTextureArea<long double>(hdrTexture);
    calculateLights();
}

IBLSampler::~IBLSampler() {
}

void IBLSampler::changeNumLights(int numLights) {
    this->numLights = numLights;
    calculateLights();
}

void IBLSampler::calculateLights() {
    // Use median cut algorithm here
    /*  n => log2(numLights)
    1.  Add the entire light probe image to the region list as a single region.
    2.  For each region in the list, subdivide along the longest dimension such that its light energy is divided evenly.
    3.  If the number of iterations is less than n, return to step 2.
    4.  Place a light source at the energy centroid of each region, 
        and set the light source color to the sum of pixel values within the region.
    5.  Convert equirectangular light sources to spherical light sources, for cube map rendering.
    */

    int n = log2(numLights);

    // Step 1
    std::vector<Region> regions;
    Region region(0, 0, hdrTexture->getWidth(), hdrTexture->getHeight());
    
    // Step 2, 3
    medianCut(region, n, regions);
    int equatorHeight = hdrTexture->getHeight() / 2;

    // Step 4, 5
    lights.clear();
    for (const Region& region : regions) {
        int x = region.x;
        int y = region.y;
        float inclinationAngle = glm::pi<float>() * (y + (region.height / 2.0f)) / hdrTexture->getHeight() - glm::pi<float>() / 2.0f; // In radians
        float cosInclinationAngle = cos(inclinationAngle);
        
        int width = region.width;
        int inclinedWidth = width * cosInclinationAngle;
        int height = region.height;
        // Color is all pixel values added up
        Vector3 lightColor(0.0f);
        for (int j = y; j < y + height; ++j) {
            for (int i = x; i < x + width; ++i) {
                Vector3 pixel = hdrTexture->getPixel(i, j);
                lightColor += pixel;
            }
        }
        Light* light = new Light();
        Vector2 centroid = Vector2(x + inclinedWidth / 2, y + height / 2);
        light->position = equirectangularToCubemapProjection(Vector2(x + inclinedWidth / 2, y + height / 2), hdrTexture->getWidth(), hdrTexture->getHeight());
        light->color = lightColor * Vector3(cosInclinationAngle);
        light->intensity = 1.0f;
        lights.push_back(light);

        // Print the light source
        // std::cout << "Light source " << lights.size() << ":" << std::endl;
        // std::cout << "Rect: (" << x << ", " << y << ", " << width << ", " << height << ")" << std::endl;
        // std::cout << "Position: "; printVector3(light->position);
        // std::cout << "Color: "; printVector3(light->color);
        // std::cout << "Intensity: " << light->intensity << std::endl;

    }
}

void IBLSampler::medianCut(Region& region, int depth, std::vector<Region>& regions) {
    if (depth == 0) {
        regions.push_back(region);
        return;
    }

    int x = region.x;
    int y = region.y;
    int width = region.width;
    int height = region.height;

    // Find the longest dimension
    float inclinationAngle = glm::pi<float>() * (y + (region.height / 2.0f)) / hdrTexture->getHeight() - glm::pi<float>() / 2.0f; // In radians
    float inclinedWidth = width * cos(inclinationAngle);
    float inclinedHeight = height;
    bool verticalCut = inclinedWidth > inclinedHeight;

    // std::cout << "Before median cut:" << std::endl;
    // std::cout << "Rect: (" << x << ", " << y << ", " << width << ", " << height << ")" << " cut: " << (verticalCut ? "VERTICAL" : "HORIZONTAL") << std::endl;

    // Find the median position along the longest dimension
    auto energy = summedTextureArea->getArea(region);
    auto halfEnergy = energy / 2.0f;
    // Use binary search to find the median position
    int left = 0.0f;
    int median = 0.0f;
    int right = verticalCut ? width : height;
    while (left <= right) {
        median = (left + right) / 2;
        Region r1 = region;
        Region r2 = region;
        if (verticalCut) { // VERTICAL
            r1.width = median;
            r2.width = width - median;
            r2.x = x + median;
        }
        else { // HORIZONTAL
            r1.height = median;
            r2.height = height - median;
            r2.y = y + median;
        }
        
        auto leftEnergy = summedTextureArea->getArea(r1);
        auto rightEnergy = summedTextureArea->getArea(r2);

        // std::cout << "R1 rect: (" << r1.x << ", " << r1.y << ", " << r1.width << ", " << r1.height << ") energy: " << leftEnergy << std::endl;
        // std::cout << "R2 rect: (" << r2.x << ", " << r2.y << ", " << r2.width << ", " << r2.height << ") energy: " << rightEnergy << std::endl;
        // std::cout << std::endl;

        if (leftEnergy <= halfEnergy && rightEnergy <= halfEnergy) {
            break;
        }
        else if (leftEnergy > halfEnergy) {
            right = median - 1;
        }
        else {
            left = median + 1;
        }
    }

    // Split the region along the longest dimension
    Region r1 = region;
    Region r2 = region;

    if (verticalCut) { // VERTICAL
        r1.width = median;
        r2.width = width - median;
        r2.x = x + median;
    }
    else { // HORIZONTAL
        r1.height = median;
        r2.height = height - median;
        r2.y = y + median;
    }

    // std::cout << "After median cut:" << std::endl;
    // std::cout << "Rect1: (" << r1.x << ", " << r1.y << ", " << r1.width << ", " << r1.height << ")" << std::endl;
    // std::cout << "Rect2: (" << r2.x << ", " << r2.y << ", " << r2.width << ", " << r2.height << ")" << std::endl;
    // std::cout << std::endl;

    // Recurse
    medianCut(r1, depth - 1, regions);
    if(r1 != r2) medianCut(r2, depth - 1, regions);
}

void IBLSampler::updateLighting() {
    calculateLights();
}

template <typename T>
SummedTextureArea<T>::SummedTextureArea(Texture* texture) {
    width = texture->getWidth();
    height = texture->getHeight();
    containers = new std::vector<SummedTextureAreaContainer<T>*>();
    calculateSummedAreaTable(texture);
}

template <typename T>
SummedTextureArea<T>::~SummedTextureArea() {
    for (SummedTextureAreaContainer<T>* container : containers) {
        delete container;
    }
    delete containers;
}

template <typename T>
T SummedTextureArea<T>::getArea(const Region& region) {
    int startX = region.x;
    int startY = region.y;
    int endX = region.x + region.width;
    int endY = region.y + region.height;

    auto A = getArea(startX - 1, startY - 1); // Top left
    auto B = getArea(endX - 1, startY - 1); // Top right
    auto C = getArea(startX - 1, endY - 1); // Bottom left
    auto D = getArea(endX - 1, endY - 1); // Bottom right

    return D - B - C + A;
}

template <typename T>
T SummedTextureArea<T>::getArea(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0.0f;
    }

    // Find the containers that contain the needed area
    bool isRegionOnLeft; // If point is on the left of the region
    bool isRegionOnTop; // If point is on the top of the region
    T area = 0.0f;
    for (SummedTextureAreaContainer<T>* container : *containers) {
        Region region = container->getRegion();
        if (x > region.x && y > region.y) {

            int localX = x - region.x;
            int localY = y - region.y;
            // Clamp the local coordinates
            localX = glm::clamp(localX, 0, region.width - 1);
            localY = glm::clamp(localY, 0, region.height - 1);
            area += container->getArea(localX, localY);
        }
    }

    return area;
}

template <typename T>
void SummedTextureArea<T>::calculateSummedAreaTable(Texture* texture) {
    // FORMULA: Y = (0.2125)R + (0.7154)G + 0.0721B
    float rWeight = 0.2125f;
    float gWeight = 0.7154f;
    float bWeight = 0.0721f;
    
    // Create region containers
    int regionWidth = 32;
    int regionHeight = 32;
    for (int y = 0; y < height; y += regionHeight) {
        for (int x = 0; x < width; x += regionWidth) {
            Region region(x, y, regionWidth, regionHeight);
            SummedTextureAreaContainer<T>* container = new SummedTextureAreaContainer<T>(region, texture);
            containers->push_back(container);
        }
    }
}

template <typename T>
SummedTextureAreaContainer<T>::SummedTextureAreaContainer(Region region, Texture* texture)
    : region(region)
{
    float rWeight = 0.2125f;
    float gWeight = 0.7154f;
    float bWeight = 0.0721f;
    summedAreaTable = new std::vector<T>(region.width * region.height, 0.0f);


    // Calculate the summed area table
    int width = region.width;
    int height = region.height;
    int xStart = region.x;
    int yStart = region.y;
    int xEnd = xStart + width;
    int yEnd = yStart + height;
    
    for (int y = yStart; y < yEnd; ++y) {
        for (int x = xStart; x < xEnd; ++x) {
            Vector3 pixel = texture->getPixel(x, y);
            T area = rWeight * pixel.x + gWeight * pixel.y + bWeight * pixel.z;
            
            if (x > xStart && y > yStart) // Diagonal
            {
                area += summedAreaTable->at((y - yStart - 1) * width + (x - xStart - 1));
            }
            if (x > xStart) // Left
            {
                area += summedAreaTable->at((y - yStart) * width + (x - xStart - 1));
            }
            if (y > yStart) // Top
            {
                area += summedAreaTable->at((y - yStart - 1) * width + x - xStart);
            }
            summedAreaTable->at((y - yStart) * width + (x - xStart)) = area;
            // Check for overflow
            if (area < 0 || area > std::numeric_limits<T>::max()) {
                std::cout << "Overflow at (" << x << ", " << y << ")" << std::endl;
            }
        }
    }
}

Vector3 equirectangularToCubemapProjection(const Vector2& v, int width, int height) {
    // Convert equirectangular coordinates to cubemap coordinates

    // Normalize the equirectangular coordinates
    float u = v.x / width;
    float v_ = -v.y / height;

    float PI = glm::pi<float>();

    // Convert the normalized coordinates to spherical coordinates
    float theta = 2.0f * PI * u;
    float phi = PI * v_;

    // Convert the spherical coordinates to cartesian coordinates
    float x = -cos(theta) * sin(phi);
    float y = -cos(phi);
    float z = sin(theta) * sin(phi);

    return -Vector3(x, y, z);
}