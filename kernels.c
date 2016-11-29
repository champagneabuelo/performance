/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "poopy butthole",              /* Team name */

    "Jason Valenzuela",     /* First member full name */
    "jev4zs@virginia.edu",  /* First member email address */

    "Isaac Park",                   /* Second member full name (leave blank if none) */
    "ip2ju@virginia.edu"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/
/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Another version of rotate
 */

/* #define iblock 32 */
#define blocksize 32
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
/* approach for transposing (from class notes)
 * > for each block: Bi, Bj
 *	> for each cell in block: i, j
 * 		> do stuff
 * 		b[j, i] = a[i, j]
 *
 */
	/* loop variables*/
	int i, j, Bi, Bj;
	int k = dim - 1;
	
	for (Bi = 0; Bi < dim; Bi += blocksize) {
		for (Bj = 0; Bj < dim; Bj += blocksize) {
			for (i = 0; i < blocksize; i++) {
				for (j = 0; j < blocksize; j++) {
					dst[RIDX(k - Bi - i, Bj + j, dim)] = src[RIDX(Bj + j, Bi + i, dim)];
				}
			}
		}
	}

}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Another version of smooth. 
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
	int i, j;
	for (i = 0; i < dim; i++) 
	{
		for (j = 0; j < dim; j++) 
		{
			int ii, jj;
			ii_max = max(i-1, 0);	
			ii_min = min(i+1, dim-1);
			jj_max = max(j-1, 0);	
			jj_min = min(j+1, dim-1);
			pixel_sum psum;
			pixel curr_pixel;
			psum.red = psum.blue = psum.greem = psum.num = 0;
			for (ii = ii_max; ii <= ii_min; ii++)
			{
				for (jj = jj_max; jj <= jj_min; jj++) 
				{
					pixel p = src[RIDX(ii, jj, dim)];
					psum.red += (int) p.red;
					psum.blue += (int) p.blue;
					psum.green += (int) p.green;
					psum.num++;
				}
				curr_pixel.red = (int) (psum.red/psum.num);
				curr_pixel.blue = (int) (psum.blue/psum.num);
				curr_pixel.green = (int) (psum.green/psum.num);
				dst[RIDX(i,j,dim)] = curr_pixel;
			}
		}
	}
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

