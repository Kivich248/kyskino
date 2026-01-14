#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"

#include <math.h>

// Пиксель: RGB, каждый компонент — uint8_t (0..255)
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Pixel;

// Изображение
typedef struct
{
	int width;   // количество столбцов (x)
	int height;  // количество строк (y)
	Pixel** pixels;  // pixels[y][x] — доступ к пикселю
	Pixel* data;     // указатель на единый блок памяти (для освобождения)
} Image;

typedef struct
{
	float** core;  // матрица ядра
	int size;        // размер ядра (обычно 3, 5, 7...)
	float divisor;   // делитель для нормализации
} Core;

Image* Negro(Image* img)
{
	int width = img->width;
    int height = img->height;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t r = img->pixels[y][x].r;
            uint8_t g = img->pixels[y][x].g;
            uint8_t b = img->pixels[y][x].b;
            img->pixels[y][x].r = 255 - r;
            img->pixels[y][x].g= 255 - g;
            img->pixels[y][x].b = 255 - b;
        }
    }
}

Image* Grey(Image* img)
{
    int width = img->width;
    int height = img->height;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t r = img->pixels[y][x].r;
            uint8_t g = img->pixels[y][x].g;
            uint8_t b = img->pixels[y][x].b;
            float x = 0,299 * r + 0,587 * g + 0,114 * b;
            img->pixels[y][x].r = round(x);
            img->pixels[y][x].g = round(x);
            img->pixels[y][x].b = round(x);
        }
    }
}
Core* create_sharp_x_core() //функция по применению фильтра резкость
{
    Core* Core = malloc(sizeof(Core));
    if (Core == NULL) return NULL;
    
    Core->size = 3;
    Core->divisor = 0.0f;
    
    // Создаем матрицу ядра
    Core->core = malloc(Core->size * sizeof(float*));
    if (Core->core == NULL) {
        free(Core);
        return NULL;
    }
    
    for (int i = 0; i < Core->size; i++) {
        Core->core[i] = malloc(Core->size * sizeof(float));
        if (Core->core[i] == NULL) {
            for (int j = 0; j < i; j++) free(Core->core[j]);
            free(Core->core);
            free(Core);
            return NULL;
        }
    }
    
   
    float sobel_x[3][3] = {    //матрица фильтра
        {0.0f, -1.0f, 0.0f},
        {-1.0f, 5.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}
    };
    
    for (int i = 0; i < Core->size; i++) {
        for (int j = 0; j < Core->size; j++) {
            Core->core[i][j] = sobel_x[i][j];
        }
    }
    
    return Core;
}


Core* create_edge_x_core()   //функция применения фильра edge
{
    Core* Core = malloc(sizeof(Core));
    if (Core == NULL) return NULL;
    
    Core->size = 3;
    Core->divisor = 0.0f;
    
    // Создаем матрицу ядра
    Core->core = malloc(Core->size * sizeof(float*));
    if (Core->core == NULL) {
        free(Core);
        return NULL;
    }
    
    for (int i = 0; i < Core->size; i++) {
        Core->core[i] = malloc(Core->size * sizeof(float));
        if (Core->core[i] == NULL) {
            for (int j = 0; j < i; j++) free(Core->core[j]);
            free(Core->core);
            free(Core);
            return NULL;
        }
    }
    
    
    float sobel_x[3][3] = { //матрица фильра
        {0.0f, -1.0f, 0.0f},
        {-1.0f, 4.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}
    };
    
    for (int i = 0; i < Core->size; i++) {
        for (int j = 0; j < Core->size; j++) {
            Core->core[i][j] = sobel_x[i][j];
        }
    }
    
    return Core;
}

Core* create_gauss_x_core() //функция применения размытия гаусса
{
    Core* Core = malloc(sizeof(Core));
    if (Core == NULL) return NULL;
    
    Core->size = 7;
    Core->divisor = 1003.0f;
    
    // Создаем матрицу ядра
    Core->core = malloc(Core->size * sizeof(float*));
    if (Core->core == NULL) {
        free(Core);
        return NULL;
    }
    
    for (int i = 0; i < Core->size; i++) {
        Core->core[i] = malloc(Core->size * sizeof(float));
        if (Core->core[i] == NULL) {
            for (int j = 0; j < i; j++) free(Core->core[j]);
            free(Core->core);
            free(Core);
            return NULL;
        }
    }
    
    
    float sobel_x[3][3] = {                                         //матрица фильтра
        { 0.0f,   0.0f,   1.0f,   2.0f,   1.0f,   0.0f,   0.0f },
		{ 0.0f,   3.0f,  13.0f,  22.0f,  13.0f,   3.0f,   0.0f},
		{ 1.0f,  13.0f,  59.0f,  97.0f,  59.0f,  13.0f,   1.0f },
		{ 2.0f,  22.0f,  97.0f, 159.0f,  97.0f,  22.0f,   2.0f },
		{ 1.0f,  13.0f,  59.0f,  97.0f,  59.0f,  13.0f,   1.0f },
		{ 0.0f,   3.0f,  13.0f,  22.0f,  13.0f,   3.0f,   0.0f },
		{ 0.0f,   0.0f,   1.0f,   2.0f,   1.0f,   0.0f,   0.0f },
    };
    
    for (int i = 0; i < Core->size; i++) {
        for (int j = 0; j < Core->size; j++) {
            Core->core[i][j] = sobel_x[i][j];
        }
    }
    
    return Core;
}
