#include <stdio.h>      // NULL, FILE
#include <string.h>     // memset()

#include "png.h"        // libpng

#include "bitmap.h"


/**
 * Initializes and clears the bitmap.
 *
 * @param w  width
 * @param h  height
 *
 * @return true=OK, false=no memory
 */
bool bitmap::init(int w, int h)
{
    width = w;
    height = h;
    offs = ((w+31)&~31)>>3;

	delete[] data;
    data = new unsigned char[height*offs];
    if (data==NULL)
        return false;

    memset(data, 0, height*offs);
    return true;
}



/**
 * Initializes the bitmap from a PNG file of 1 bit depth.
 *
 * @param filename  the PNG-file to be read
 *
 * @return 0=OK, -1=can't read file, -2=no memory, -3=PNG error, -4=wrong depth
 */
int bitmap::init_from_png(const char* filename)
{
    png_structp png_ptr;
    png_infop   info_ptr;
    FILE*       fp;
    
    if ((fp = fopen(filename, "rb")) == NULL)
        return -1;
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        return -2;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return -2;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -3;
    }

    png_init_io(png_ptr, fp);
    
    png_read_info(png_ptr, info_ptr);
    
    if (png_get_bit_depth(png_ptr, info_ptr) != 1)
        return -4;
    
    // initialize bitmap
    width  = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    offs   = png_get_rowbytes(png_ptr, info_ptr);

	delete[] data;
    data = new unsigned char[height*offs];
    if (data==NULL)
        return -2;

    png_bytep* row_pointers = new png_bytep[height];
    if (row_pointers==NULL)
        return -2;

    for (int i=0; i<height; i++)
        row_pointers[i] = (png_bytep)get_row_pointer(i);
    
    png_set_rows(png_ptr, info_ptr, row_pointers);

    png_read_image(png_ptr, row_pointers);

    delete[] row_pointers;

    png_read_end(png_ptr, info_ptr);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    
    fclose(fp);
    
    return 0;
}



/**
 * Read 2x2 pixel box at (x,y).
 * @return bit(x-1,y-1)*8 + bit(x,y-1)*4 + bit(x-1,y)*2 + bit(x,y)
 */
int bitmap::get_point4(int x, int y) const
{
    unsigned char *p0;
    unsigned char *p1 = data + y*offs;
    int s0, s1;
    int index = 0;
    
    assert(x>=0 && y>=0 && x<=width && y<=height);
    
    p0 = p1 + (x>>3);
    s0 = 0x80 >> (x&7);
    x--;
    p1 += (x>>3);
    s1 = 0x80 >> (x&7);
    x++;
    
    if (y < height)
    {
        if (x<width && (*p0&s0)!=0)
            index |= 1;                 // (x,y) is set
        if (x>0 && (*p1&s1)!=0)
            index |= 2;                 // (x-1,y) is set
    }

    p0 -= offs;
    p1 -= offs;
    if (y > 0)
    {
        if (x<width && (*p0&s0)!=0)
            index |= 4;                 // (x,y-1) is set
        if (x>0 && (*p1&s1)!=0)
            index |= 8;                 // (x-1,y-1) is set
    }

    return index;
}


/**
* Search for the next set bit in this row.
* @return x..width-1 (success) or width (no success)
*/
int bitmap::next_bit_set(int x, int y) const
{
    unsigned char *p = data + y*offs + (x>>3);
    unsigned char b = *p & (0xFF>>(x&7));
    
    assert(x>=0 && x<width && y>=0 && y<height);

    x &= ~7;
    while (b == 0) {
        x += 8;
        if (x >= width)
            return width;
        b = *++p;
    }
    
    // search for set bit
    while ((b&0x80)==0 && ++x<width)
        b += b;

    return x;
}


/**
* Search for the next clear bit in this row.
* @return x..width-1 (success) or width (no success)
*/
int bitmap::next_bit_clr(int x, int y) const
{
    unsigned char *p = data + y*offs + (x>>3);
    unsigned char b = *p | (0xFF00>>(x&7));
    
    assert(x>=0 && x<width && y>=0 && y<height);

    x &= ~7;
    while (b == 0xFF) {
        x += 8;
        if (x >= width)
            return width;
        b = *++p;
    }

    // search for clear bit
    while ((b&0x80) && ++x<width)
        b += b;

    return x;
}
