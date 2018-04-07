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

int32_t getPixel(const pgm& image, int32_t x, int32_t y)
{
    int32_t index = (y*image.width)+x;
    return image.image[index];
}

void applyFilter(pgm& image, const pgm& filter)
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
            int32_t value = getPixel(image, width+1, height);
            value += getPixel(image, width-1, height);
            value += getPixel(image, width, height+1);
            value += getPixel(image, width, height-1);
            image.image[width, height] = value*filter.image[width, height];
            i++;
            j++;
        }
        i = (image.width*++row) + 1;
        width = image.width-2;
    }
}

void writeImage(const pgm& image, const char* file)
{
    std::ofstream stream;
    stream.open(file);
    stream << image.magic << "\n" << image.width << " " << image.height << "\n" << image.max << "\n";

    int32_t j = 0;
    int32_t i = (image.height*image.width);
    while(i--)
    {
         stream << (int32_t)image.image[j++] << "\n";
    }

    stream.close();
}

void getFilterWindow(int32_t x, int32_t y, const pgm& image, const pgm& filter, pgm& window)
{
    window.magic = image.magic;
    window.width = filter.width;
    window.height = filter.height;
    window.max = filter.max;

    window.image = new float[window.width*window.height];

    int32_t i = 0;
    int32_t X = x*filter.width;
    int32_t Y = y*filter.height;
    while(Y < (y+1)*filter.height)
    {
        while(X < (x+1)*filter.width)
        {
            int32_t value = getPixel(image, X++, Y);
            window.image[i++] = value;
        }
        X = x*filter.width;
        Y++;
    }
}

int32_t main(int32_t argc, char** argv)
{
    pgm filter, image, window;

    std::ifstream filterData;    
    filterData.open("filter.pgm");
    readPgm(filterData, filter);
    filterData.close();

    std::ifstream imageData;    
    imageData.open("clip.pgm");
    readPgm(imageData, image);
    imageData.close();

    buildFilter(filter);

    int32_t x = 0;
    int32_t y = 0;
    std::string name = "out";
    while(y < image.height/filter.height)
    {
        while(x < image.width/filter.width)
        {
            getFilterWindow(x, y, image, filter, window);
            std::string output = name + std::to_string(x) +"_"+ std::to_string(y) + ".pgm";
            applyFilter(window, filter);
            writeImage(window, output.c_str());
            x++;
        }
        x = 0;
        y++;
    }

    std::ofstream filteredImage;
    filteredImage.open("filtered.pgm");
    filteredImage << image.magic << "\n" << image.width << " " << image.height << "\n" << image.max << "\n";

    x = 0;
    y = 0;
    while(y < image.height/filter.height)
    {
        while(x < image.width/filter.width)
        {
            std::string output = name + std::to_string(x) + "_" + std::to_string(y) + ".pgm";
            pgm clip;
            std::ifstream clipData;
            clipData.open(output.c_str());
            readPgm(clipData, clip);
            int32_t i = 0;
            int32_t width = clip.width;
            while(clip.height--)
            {
                while(width--)
                {
                    filteredImage << clip.image[i] << "\n";
                    i++;
                }
                width = clip.width;
            }
            delete[] clip.image;
            remove(output.c_str());
            x++;
        }
        x = 0;
        y++;
    }

    delete[] window.image;
    delete[] filter.image;
    delete[] image.image;

    return 0;
}
