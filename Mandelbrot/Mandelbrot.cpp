// Mandelbrot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <omp.h>
#include "tga.h"

int xres;
int yres;
float xmax;
float xmin;
float ymax;
float ymin;
int maxN;

void paintPix(int x, int y, int n)	{
	printf("%d: Paint pixel (%d,%d) with color %d\n", omp_get_thread_num(), x, y, n);
}

void calcPix(int x, int y)	{
	int cr = xmin + (xmax - xmin) * x; // scale
	int ci = ymin + (ymax - ymin) * y; // scale
	int zr = 0;
	int zi = 0;

	for (int n = 0; n < maxN; ++n) {
		int nextZr = zr * zr - zi * zi + cr;
		int nextZi = 2 * zr * zi + ci;
		if (nextZr * nextZr + nextZi * nextZi > 4) {
			paintPix(x, y, n);
			return;
		}
	}
	paintPix(x, y, 0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)	{
		printf("Mandelbrot - Please provide tga-filename as argument!");
		return 1;
	}

	printf("Starting parallel mandelbrot calculation!\n", omp_get_num_threads());

	// read values
	scanf_s("%d", &xres);
	scanf_s("%d", &yres);
	scanf_s("%e", &xmin);
	scanf_s("%e", &ymin);
	scanf_s("%e", &xmax);
	scanf_s("%e", &ymax);
	scanf_s("%d", &maxN);

	#pragma omp parallel
	{
		for (int x = omp_get_thread_num(); x < xres; x += omp_get_num_threads())	{
			for (int y = 0; y < yres; y += 1)	{
				calcPix(x, y);
			}
		}
	}

	scanf_s("%d", &maxN);
	return 0;
}