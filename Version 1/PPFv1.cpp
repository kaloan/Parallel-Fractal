#include <iostream>
#include <cstdint>
#include <thread>
#include <complex>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

using namespace std;

//Maximum number of iterations before we consider the point to lie in the fractal set
//Also the number of discrete color intensities
#define MAXITERS 256


//RGB structure
struct RGB
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};


//Coloring function
void colorPixel(RGB& pixel, int& iterNum)
{
	/*
	if (iterNum >= MAXITERS)
	{
		pixel.red = 0;
		pixel.green = 0;
		pixel.blue = 0;
	}
	else if (iterNum >= MAXITERS/2)
	{
		pixel.red = 2 * (MAXITERS - iterNum - 1);
		pixel.green = MAXITERS - iterNum - 1;
		pixel.blue = (MAXITERS - iterNum) / 2;
	}
	else if (iterNum >= MAXITERS / 4)
	{
		pixel.red = iterNum;
		pixel.green = iterNum / 2;
		pixel.blue = MAXITERS - 2 * iterNum;
	}
	else
	{
		pixel.red = iterNum / 2;
		pixel.green = 2 * iterNum;
		pixel.blue = MAXITERS - 3 * iterNum;
	}
	*/
	/*
	pixel.red = (MAXITERS - iterNum);
	pixel.green = (MAXITERS - iterNum);
	pixel.blue = (MAXITERS - iterNum);
	*/


	pixel.red = ((int)(iterNum * sinf(iterNum)) % MAXITERS);
	pixel.green = ((iterNum*iterNum) % MAXITERS);
	pixel.blue = (iterNum*iterNum*iterNum) % MAXITERS;

}


//Main calculations
void calculateFractal(int& width, int& height, int& numThreads, int& granularity, int thisThreadNum, RGB**& fractal,
	long double& xmin, long double& xmax, long double& ymin, long double& ymax,
	long double& xStepSize, long double& yStepSize, long double& maxNorm, complex<long double>& centre)
{
	//Starting thread
	clock_t threadStart = clock();
	printf("Thread-%d started.\n", thisThreadNum + 1);


	for (int i = granularity * thisThreadNum; i < height; i += granularity * numThreads)
	{
		for (int k = 0; k < granularity; k++)
		{
			for (int j = 0; j < width; j++)
			{
				complex<long double> pixelCoordinates(xmin + ((long double)j + 0.5)*xStepSize, ymax - ((long double)(i + k) + 0.5)*yStepSize);
				complex<long double> ziter(0, 0);
				int iterNum = 0;
				do
				{
					ziter = exp(ziter*ziter + pixelCoordinates);
					//ziter = ziter * ziter + pixelCoordinates;
					iterNum++;
				} while (iterNum < MAXITERS && abs(ziter - centre) <= maxNorm);
				colorPixel(fractal[i + k][j], iterNum);
			}
		}
	}


	printf("Thread-%d stopped.\n", thisThreadNum + 1);
	clock_t threadLifeTime = clock() - threadStart;
	printf("Thread-%d execution time was %.0f,(millis).\n", thisThreadNum + 1, 1000 * (float)threadLifeTime / CLOCKS_PER_SEC);
}


//Main
int main(int argc, char* argv[])
{
	//Starting timer
	clock_t totalStart = clock();


	//Initializing default options
	//char *filename = (char *)"zad16-work.ppm";
	char* filename = new char[100];
	strcpy_s(filename, 100, "zad16-work.ppm");
	bool quiet = false;
	int width = 640;
	int height = 480;
	long double xmin = -2.0;
	long double xmax = 2.0;
	long double ymin = -2.0;
	long double ymax = 2.0;
	int numThreads = 1;
	int granularity = 1;

	//Analyzing command options
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-q") == 0)
		{
			quiet = true;
		}
		if (strcmp(argv[i], "-s") == 0)
		{
			sscanf_s(argv[i + 1], "%dx%d", &width, &height);
		}
		if (strcmp(argv[i], "-r") == 0)
		{
			sscanf_s(argv[i + 1], "%Lf:%Lf:%Lf:%Lf", &xmin, &xmax, &ymin, &ymax);
		}
		if (strcmp(argv[i], "-t") == 0)
		{
			sscanf_s(argv[i + 1], "%d", &numThreads);
		}
		if (strcmp(argv[i], "-g") == 0)
		{
			sscanf_s(argv[i + 1], "%d", &granularity);
		}
		if (strcmp(argv[i], "-o") == 0)
		{
			sscanf_s(argv[i + 1], "%s", filename, 100);
			//strcpy(filename, argv[i + 1]);
		}
	}

	//Calculate step sizes and maximum norm
	long double xStepSize = (xmax - xmin) / (long double)width;
	long double yStepSize = (ymax - ymin) / (long double)height;
	long double maxNorm = 0.5*sqrt((xmax - xmin)*(xmax - xmin) + (ymax - ymin)*(ymax - ymin));
	complex<long double> centre = 0.5*(xmin + xmax, ymin + ymax);
	//long double maxNorm = 2.0;

	//Threads
	//vector<thread> threads(numThreads);
	vector<thread> threads;
	threads.reserve(numThreads);

	//2D RGB Array
	RGB** fractal = new RGB*[height];
	fractal[0] = new RGB[height*width];
	for (int i = 1; i < height; i++)
	{
		fractal[i] = fractal[i - 1] + width;
	}

	//Start threads
	for (int i = 0; i < numThreads; i++)
	{
		//threads[i] = thread{ calculateFractal, ref(width), ref(height), ref(numThreads), i , ref(fractal), 
		//	ref(xmin), ref(xmax), ref(ymin), ref(ymax), ref(xStepSize), ref(yStepSize), ref(maxNorm) };
		threads.emplace_back(calculateFractal, std::ref(width), std::ref(height), std::ref(numThreads), std::ref(granularity), i, std::ref(fractal),
			std::ref(xmin), std::ref(xmax), std::ref(ymin), std::ref(ymax), std::ref(xStepSize), std::ref(yStepSize), std::ref(maxNorm), std::ref(centre));
	}

	//Join threads
	for (int i = 0; i < numThreads; i++)
	{
		if (threads[i].joinable()) threads[i].join();
	}



	//Create the ppm file

	FILE* fd;
	fopen_s(&fd, filename, "w");

	//Write the header
	fprintf(fd, "P6\n");
	fprintf(fd, "%d %d\n", width, height);
	fprintf(fd, "%d\n", MAXITERS - 1);

	//Write the data and exit file
	fwrite(fractal[0], 3 * width, height, fd);
	fclose(fd);

	delete fractal[0];
	delete fractal;

	//Returning the total execution time
	clock_t totalElapsed = clock() - totalStart;
	printf("Total execution time for current run (millis): %.0f\n", 1000.0 * (float)totalElapsed / CLOCKS_PER_SEC);

	return 0;
}
