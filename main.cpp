/*
 * Copyright Blake W. Ford 2018
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 */

#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fstream>

enum filterType
{
    LeftRight,
    UpDown,
    LeftRight3Bar,
    UpDown3Bar,
    Checker
};

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

    int32_t i = 0;
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
            setPixel(buffer, width, height, (value*mask) >= 255 ? 255: (value*mask));
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

void writeWindowImage(const pgm& image, const char* file)
{
    std::ofstream stream;
    stream.open(file);
    stream << image.magic << "\n" << image.width << " " << image.height << "\n" << image.max << "\n";

    int32_t j = 0;
    const int32_t diff = 16;
    int32_t i = (image.height*image.width);
    while(i--)
    {
         //AddHardCodedValueToReport
         stream << ((image.image[j] > 128-diff) && (image.image[j] < 128+diff) ? image.image[j]: 255) << "\n";
         j++;
    }

    stream.close();
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
         stream << image.image[j] << "\n";
         j++;
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
            writeWindowImage(window, output.c_str());
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
        command.append("-compress none +append " + file);
        system(command.c_str());
        i++;
        x=0;
        y+=filter.height;
    }

    system("rm out*.pgm");

    std::ofstream filtered;

    filtered.open("filtered.pgm");
    filtered << image.magic << "\n" << image.width << " " << image.height << "\n" << image.max << "\n";
    for(int j = 0; j < i; j++)
    {
        pgm layer;
        std::ifstream temp;
        std::string file = "temp" + std::to_string(j) + ".pgm";
        temp.open(file.c_str());
        readPgm(temp, layer);
        int32_t count = layer.width*layer.height;
        for(int i=0; i < count; i++)
        {
            filtered << layer.image[i] << "\n";
        }
    }

    system("rm temp*.pgm");
    filtered.close();
}

void startBuildingFilter(int32_t width, int32_t height, pgm& filter)
{
    filter.magic = "P2";
    filter.width = width;
    filter.height = height;
    filter.max = 255;
    filter.image = new float[width*height];
}

void buildLeftRightFilter(int32_t width, int32_t height, pgm& filter, bool inverse)
{
    startBuildingFilter(width, height, filter);

    int32_t x = 0;
    int32_t y = 0;
    while(y < filter.height)
    {
        while(x < filter.width)
        {
            if(x < filter.width/2)
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            else
            {
                setPixel(filter, x, y, inverse ? 0.0f: 255.0f);
            }
            x++;
        }
        x=0;
        y++;
    }

}

void buildUpDownFilter(int32_t width, int32_t height, pgm& filter, bool inverse)
{
    startBuildingFilter(width, height, filter);

    int32_t x = 0;
    int32_t y = 0;
    while(y < filter.height)
    {
        while(x < filter.width)
        {
            if(y < filter.height/2)
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            else
            {
                setPixel(filter, x, y, inverse ? 0.0f: 255.0f);
            }
            x++;
        }
        x=0;
        y++;
    }

}

void buildLeftRight3BarFilter(int32_t width, int32_t height, pgm& filter, bool inverse)
{
    startBuildingFilter(width, height, filter);

    int32_t x = 0;
    int32_t y = 0;
    while(y < filter.height)
    {
        while(x < filter.width)
        {
            if(x < filter.width/3)
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            else if(x < (filter.width/3)*2)
            {
                setPixel(filter, x, y, inverse ? 0.0f: 255.0f);
            }
            else
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            x++;
        }
        x=0;
        y++;
    }

}

void buildUpDown3BarFilter(int32_t width, int32_t height, pgm& filter, bool inverse)
{
    startBuildingFilter(width, height, filter);

    int32_t x = 0;
    int32_t y = 0;
    while(y < filter.height)
    {
        while(x < filter.width)
        {
            if(y < filter.height/3)
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            else if(y < (filter.height/3)*2)
            {
                setPixel(filter, x, y, inverse ? 0.0f: 255.0f);
            }
            else
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            x++;
        }
        x=0;
        y++;
    }

}

void buildCheckerFilter(int32_t width, int32_t height, pgm& filter, bool inverse)
{
    startBuildingFilter(width, height, filter);

    int32_t x = 0;
    int32_t y = 0;
    while(y < filter.height)
    {
        while(x < filter.width)
        {
            if(x < filter.width/2 && y < filter.height/2)
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            else if(x >= filter.width/2 && y < filter.height/2)
            {
                setPixel(filter, x, y, inverse ? 0.0f: 255.0f);
            }
            else if(x < filter.width/2 && y >= filter.height/2)
            {
                setPixel(filter, x, y, inverse ? 0.0f: 255.0f);
            }
            else
            {
                setPixel(filter, x, y, inverse ? 255.0f: 0.0f);
            }
            x++;
        }
        x=0;
        y++;
    }

}

void test(const pgm& image, pgm& filter)
{
    const int32_t nFactors=7;
    //AddHardCodedValueToReport
    int32_t factors[nFactors] = {2,4,5,10,20,25,50};

    int32_t k=2;
    bool inverse=false;
    while(k--)
    {
        int32_t i=nFactors;
        int32_t j=nFactors;
        while(i--)
        {
            while(j--)
            {
//                buildLeftRightFilter(factors[j], factors[i], filter, inverse);
//                buildUpDownFilter(factors[j], factors[i], filter, inverse);
//                buildLeftRight3BarFilter(factors[j], factors[i], filter, inverse);
//                buildUpDown3BarFilter(factors[j], factors[i], filter, inverse);
//                buildCheckerFilter(factors[j], factors[i], filter, inverse);

                buildFilter(filter);
                applyWholeImageFilter(image, filter);
                reconstructImage(image, filter);

                usleep(1000*1000*5);
                delete[] filter.image;
            }            
            j=nFactors;
        }
        inverse=true;        
    }
}

void getProductionFilterDimensions(filterType type, int32_t& width, int32_t& height)
{
    switch(type)
    {
        case LeftRight:
            width = 10;
            height = 50;
            break;
        case UpDown:
            width = 50;
            height = 5;
            break;
        case LeftRight3Bar:
            width = 10;
            height = 50;
            break;
        case UpDown3Bar:
            width = 50;
            height = 5;
            break;
        case Checker:
            width = 10;
            height = 50;
            break;
        default:
            exit(-1);
    }
}

void production(filterType type, const pgm& image, pgm& filter)
{
    //AddHardCodedValueToReport
    switch(type)
    {
        case LeftRight:
            buildLeftRightFilter(10, 50, filter, false);
            break;
        case UpDown:
            buildUpDownFilter(50, 5, filter, true);
            break;
        case LeftRight3Bar:
            buildLeftRight3BarFilter(10, 50, filter, false);
            break;
        case UpDown3Bar:
            buildUpDown3BarFilter(50, 5, filter, true);
            break;
        case Checker:
            buildCheckerFilter(10, 50, filter, false);
            break;
        default:
            exit(-1);
    }

    buildFilter(filter);
    applyWholeImageFilter(image, filter);
    reconstructImage(image, filter);
}

void convertImage(const char* path)
{
    char buffer[256];
    memset(buffer, '\0', 256);
    sprintf(buffer, "convert -compress none %s clip.pgm", path);
    system(buffer);
}

void trainingPass(bool detected, float score)
{
    //AddHardCodedValueToReport
    //Range
    //printf("%.2f\n", score);
    //Accuracy
    if(detected && score > 5.0f && score < 30.0f)
        printf("1+");
    else
        printf("0+");
}

void faceTest(filterType type, const pgm& image)
{
    pgm window;
    //AddHardCodedValueToReport
    window.width = 100;
    window.height = 100;
    window.max = 255;

    int32_t x = 0;
    int32_t y = 0;
    std::string name = "sample";
    while(y < image.height)
    {
        while(x < image.width)
        {
            getFilterWindow(x, y, image, window, window);
            std::string output = name + std::to_string(x) +"_"+ std::to_string(y) + ".pgm";
//            writeWindowImage(window, output.c_str());

            int32_t width = window.width;
            int32_t height = window.height;
            int32_t i = 0;
            int32_t score2 = 0;
            int32_t accumulator = 0;
            int32_t accumulator2 = 0;
            while(height--)
            {
                while(width--)
                {
                   if(window.image[i] != 255.0f)
                       accumulator += window.image[i];
                    accumulator2 += window.image[i];
                    i++;
                }
                if((height > 85) && (accumulator2 == 25500))
                {
                    score2++;
                }
                if((height < 85 && height > 80) && (accumulator2 > 24500 && accumulator2 < 25500))
                {
                    score2++;
                }
                if((height < 65 && height > 60) && (accumulator2 > 23000 && accumulator2 < 25000))
                {
                    score2++;
                }
                if((height < 45 && height > 40) && (accumulator2 < 25000))
                {
                    score2++;
                }
                if((height < 25 && height > 20) && (accumulator2 < 25500))
                {
                    score2++;
                }
                accumulator2 = 0;
                width = window.width;
            }

            float score = accumulator/(window.width*window.height);

            bool detected = false;
            const float low = 1.0f;
            //AddHardCodedValueToReport
            switch(type)
            {
                case LeftRight:
                    detected = (score > low && score < 12.0f);
                    break;
                case UpDown:
                    detected = (score > low && score < 8.0f);
                    break;
                case LeftRight3Bar:
                    detected = (score > low && score < 12.0f);
                    break;
                case UpDown3Bar:
                    detected = (score > low && score < 8.0f);
                    break;
                case Checker:
                    detected = (score > low && score < 10.0f);
                    break;
                default:
                    exit(-1);
            }
            trainingPass(detected, score2);

            if(detected)
            {
//                printf("Face detected\n");
            }

            delete[] window.image;
            x+=window.width;
        }
        x=0;
        y+=window.height;
    }

#ifdef CHECK
     printf("0");
#endif
}

#ifdef LR
filterType type = LeftRight;
#endif
#ifdef UD
filterType type = UpDown;
#endif
#ifdef LR3
filterType type = LeftRight3Bar;
#endif
#ifdef UD3
filterType type = UpDown3Bar;
#endif
#ifdef CHECK
filterType type = Checker;
#endif

int32_t main(int32_t argc, char** argv)
{
    if(argc != 2)
    {
        printf("usage: adaboost <image>\n");
        return -1;
    }

    convertImage(argv[1]);

    pgm filter, image;

    std::ifstream imageData;
    imageData.open("clip.pgm");
    readPgm(imageData, image);
    imageData.close();

    int32_t width, height;
    getProductionFilterDimensions(type, width, height);
    image.width  -= image.width%width;
    image.height -= image.height%height;

#if 0
    test(image, filter);
#else
    production(type, image, filter);
#endif

    pgm filtered;
    std::ifstream filteredData;
    filteredData.open("filtered.pgm");
    readPgm(filteredData, filtered);
    filteredData.close();

    faceTest(type, filtered);

    return 0;
}
