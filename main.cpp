/*
 * Copyright Blake W. Ford 2018
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 */

#include <stdint.h>
#include <fstream>

struct pgm
{
    std::string magic;
    int32_t width, height, max;
    float* image = nullptr;
};

void readPgm(std::ifstream& stream, pgm& image)
{
    stream >> image.magic >> image.width >> image.height >> image.max;

    int32_t pixel;
    int32_t width = image.width;
    int32_t height = image.height;
//    printf("%s %d %d %d\n", image.magic.c_str(), image.width, image.height, image.max);
    image.image = new float[image.width*image.height];

    int32_t i = 0;
    while(height--)
    {
        while(width--)
        {
            stream >> pixel;
            image.image[i] = pixel;
//            printf("%.2f\n", image.image[i]);
            i++;
        }
        width = image.width;
    }
}

void buildFilter(pgm& image)
{
    int32_t width = image.width;
    int32_t height = image.height;
    float elements = image.width*image.height;

    int32_t i;
    while(height--)
    {
        while(width--)
        {
            image.image[i] /= elements;
//            printf("%.2f\n", image.image[i]);
            i++;
        }
        width = image.width;
    }
}

void applyFilter(pgm& image)
{
//    printf("%s %d %d %d\n", image.magic.c_str(), image.width-2, image.height-2, image.max);

    int32_t width = image.width-2;
    int32_t height = image.height-2;

    int32_t j = 0;
    int32_t row = 0;
    int32_t i = (image.width*++row) + 1;
    while(height--)
    {
        while(width--)
        {
//            printf("%d\n", (int32_t)image.image[i]);
            i++;
            j++;
        }
        i = (image.width*++row) + 1;
        width = image.width-2;
    }
}

int32_t main(int32_t argc, char** argv)
{
    pgm filter, image;

    std::ifstream filterData;    
    filterData.open("filter.pgm");
    readPgm(filterData, filter);
    filterData.close();

    std::ifstream imageData;    
    imageData.open("clip.pgm");
    readPgm(imageData, image);
    imageData.close();

    buildFilter(filter);
    applyFilter(image);

    delete[] filter.image;
    delete[] image.image;

    return 0;
}
