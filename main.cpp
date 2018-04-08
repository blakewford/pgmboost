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
    image.image = new float[image.width*image.height];

    int32_t i = 0;
    while(height--)
    {
        while(width--)
        {
            stream >> pixel;
            image.image[i] = pixel;
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

float getFloatPixel(const pgm& image, int32_t x, int32_t y)
{
    int32_t index = (y*image.width)+x;
    return image.image[index];
}

void setPixel(const pgm& image, int32_t x, int32_t y, float value)
{
    int32_t index = (y*image.width)+x;
    image.image[index] = value;
}

void applyFilter(pgm& image, const pgm& filter)
{
    int32_t width = image.width-2;
    int32_t height = image.height-2;

    pgm buffer;
    buffer.width = image.width;
    buffer.height = image.height;
    buffer.image = new float[image.width*image.height];

    int32_t j = 0;
    int32_t row = 0;
    int32_t i = (image.width*++row) + 1;
    while(height--)
    {
        while(width--)
        {
            int32_t value = getPixel(image, width+1, height);
            value += getPixel(image, width-1, height);
            value += getPixel(image, width, height+1);
            value += getPixel(image, width, height-1);
            float mask = getFloatPixel(filter, width, height);
            setPixel(buffer, width, height, (value*mask) >= 128 ? 255: 127);
            i++;
            j++;
        }
        i = (image.width*++row) + 1;
        width = image.width-2;
    }

    float* oldImage = image.image;
    image.image = buffer.image;
    delete[] oldImage;
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
         image.image[0] = 255;
         image.image[1] = 255;
         stream << (image.image[j++] == 127 ? 127: 255) << "\n";
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
    int32_t count = window.width*window.height;
    while(count--)
    {
        window.image[count] = 255;
    }

    int32_t i = 0;
    int32_t X = 0;
    int32_t Y = 0;
    while(Y < filter.height)
    {
        while(X < filter.width)
        {
            int32_t value = getPixel(image, x+X, y+Y);
            window.image[i++] = value;
            X++;
        }
        X = 0;
        Y++;
    }
}

void applyWholeImageFilter(const pgm& image, const pgm& filter)
{
    pgm window;

    int32_t x = 0;
    int32_t y = 0;
    std::string name = "out";
    while(y < image.height)
    {
        while(x < image.width)
        {
            getFilterWindow(x, y, image, filter, window);
            std::string output = name + std::to_string(x) +"_"+ std::to_string(y) + ".pgm";
            applyFilter(window, filter);
            writeImage(window, output.c_str());
            x+=filter.width;
        }
        x=0;
        y+=filter.height;
    }
}

void reconstructImage(const pgm& image, const pgm& filter)
{
    int32_t i=0;
    int32_t x = 0;
    int32_t y = 0;
    std::string name = "out";
    while(y < image.height)
    {
        std::string file = "temp" + std::to_string(i) + ".pgm";
        std::string command = "convert ";
        while(x < image.width)
        {
            std::string output = name + std::to_string(x) + "_" + std::to_string(y) + ".pgm";
            command.append(output + " ");
            x+=filter.width;
        }
        command.append("-border 0 +append " + file);
        system(command.c_str());
        i++;
        x=0;
        y+=filter.height;
    }

    system("rm out*.pgm");
    system("cp temp0.pgm filtered.pgm");

    std::string command = "convert filtered.pgm ";
    for(int j = 1; j < i; j++)
    {
        std::string file = "temp" + std::to_string(j) + ".pgm ";
        command.append(file);
    }
    command.append(" -border 0 -append filtered.pgm");
    system(command.c_str());
    system("rm temp*.pgm");
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

    applyWholeImageFilter(image, filter);
    reconstructImage(image, filter);

    delete[] window.image;
    delete[] filter.image;
    delete[] image.image;

    return 0;
}
