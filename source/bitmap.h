#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <stdio.h>          // NULL
#include <assert.h>


class bitmap
{
private:
    unsigned char* data;    // NULL means uninitialized
    int width;
    int height;
    int offs;

    bitmap(const bitmap&);
    bitmap& operator=(const bitmap&);

public:
    bitmap() : data(NULL), width(0), height(0), offs(0) {}
    ~bitmap() { delete[] data; }
    
    bool init(int w, int h);
    int init_from_png(const char* filename);

    unsigned char* get_row_pointer(int i) const
    {
        assert(i>=0 && i<height);
        return data + i*offs;
    }

    int get_width() const { return width; }
    int get_height() const { return height; }

    int get_point4(int x, int y) const;
    
    void set_bit(int x, int y)
    {
        assert(x>=0 && x<width);
        assert(y>=0 && y<height);
        data[y*offs+(x>>3)] |= 0x80>>(x&7);
    }

    bool bit_is_set(int x, int y) const
    {
	    assert(x>=0 && x<width);
	    assert(y>=0 && y<height);
	    return (data[y*offs+(x>>3)]&(0x80>>(x&7)))!=0;
    }

    int next_bit_set(int x, int y) const;
    int next_bit_clr(int x, int y) const;
};

#endif
