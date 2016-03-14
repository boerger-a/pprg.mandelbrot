// Mandelbrot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <omp.h>
#include "tga.h"
#include <time.h>
#include <tuple>

int xres;
int yres;
float xmax;
float xmin;
float ymax;
float ymin;
int maxN;

tga::TGAImage* image;

std::vector<std::vector<unsigned char>> colors { 
	{ 66, 30, 15 },
	{ 25, 7, 26 },
	{ 9, 1, 47 },
	{ 4, 4, 73 },
	{ 0, 7, 100 },
	{ 12, 44, 138 },
	{ 24, 82, 177 },
	{ 57, 125, 209 },
	{ 134, 181, 229 },
	{ 211, 236, 248 },
	{ 241, 233, 191 },
	{ 248, 201, 95 },
	{ 255, 170, 0 },
	{ 204, 128, 0 },
	{ 153, 87, 0 },
	{ 106, 52, 3 }
};

std::tuple<int, int, int> get_rgb_smooth(int n, int iter_max) {
	// map n on the 0..1 interval
	double t = (double)n / (double)iter_max;

	// Use smooth polynomials for r, g, b
	int r = (int)(9 * (1 - t)*t*t*t * 255);
	int g = (int)(15 * (1 - t)*(1 - t)*t*t * 255);
	int b = (int)(8.5*(1 - t)*(1 - t)*(1 - t)*t * 255);
	return std::tuple<int, int, int>(r, g, b);
}

void paintPix(int x, int y, int n)	{
	//printf("%d: Paint pixel (%d,%d) with color for %d\n", omp_get_thread_num(), x, y, n);

	int index = (x + (yres - y - 1) * xres) * 3;

	// colors from array
	int c = n % 16;
	image->imageData[index] = colors[c][0];
	image->imageData[index + 1] = colors[c][1];
	image->imageData[index + 2] = colors[c][2];

	/*// smooth colors
	std::tuple<int, int, int> rgb = get_rgb_smooth(n, maxN);
	image->imageData[index] = std::get<0>(rgb);
	image->imageData[index + 1] = std::get<1>(rgb);
	image->imageData[index + 2] = std::get<2>(rgb);*/
}

void calcPix(int x, int y)	{
	float cr = xmin + (xmax - xmin) / (xres - 1) * x; // scale
	float ci = ymin + (ymax - ymin) / (yres - 1) * y; // scale
	float zr = 0;
	float zi = 0;

	for (int n = 0; n < maxN; ++n) {
		float nextZr = zr * zr - zi * zi + cr;
		float nextZi = 2 * zr * zi + ci;
		if ((nextZr * nextZr + nextZi * nextZi) > 4) {
			paintPix(x, y, n);
			return;
		}
		zr = nextZr;
		zi = nextZi;
	}
	paintPix(x, y, maxN);
}

int main(int argc, char *argv[])
{
	if (argc != 2)	{
		printf("Mandelbrot - Please provide tga-filename as argument!");
		return 1;
	}

	char* filepath = argv[1];

	//printf("Starting parallel mandelbrot calculation!\n", omp_get_num_threads());
	//printf("Please provide input parameters in the following order (xres, yres, xmin, ymin, xmax, ymax, maxN): \n", omp_get_num_threads());

	// read values
	scanf_s("%d", &xres);
	scanf_s("%d", &yres);
	scanf_s("%e", &xmin);
	scanf_s("%e", &ymin);
	scanf_s("%e", &xmax);
	scanf_s("%e", &ymax);
	scanf_s("%d", &maxN);

	image = new tga::TGAImage;
	image->bpp = 24;
	image->height = yres;
	image->width = xres;
	image->type = 0;
	image->imageData = std::vector<unsigned char>(3 * xres * yres);

	//clock_t start = clock(); // save start seconds

	#pragma omp parallel
	{
		for (int x = omp_get_thread_num(); x < xres; x += omp_get_num_threads())	{
			for (int y = 0; y < yres; y += 1)	{
				calcPix(x, y);
			}
		}
	}

	// print elapsed time
  	//clock_t end = clock();
	//printf("Cacluation finished in %d millis.", end - start);

	tga::saveTGA(*image, filepath);	  

	return 0;
}