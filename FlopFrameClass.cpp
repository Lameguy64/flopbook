/* 
 * File:   FlopFrameClass.cpp
 * Author: Lameguy64
 * 
 * Created on February 14, 2019, 10:20 AM
 */

#include <malloc.h>
#include <string.h>
#include <math.h>

#include "FlopFrameClass.h"

extern unsigned char tbl_cmap[];

template <typename T> int sgn(T val) {

    return (T(0) < val) - (val < T(0));

}

FlopFrameClass::FlopFrameClass(int w, int h, int bpp) {
	
	switch( bpp ) {
		case F_4BPP:
			
			p_size = (w>>1)*h;
			pixels = malloc( p_size );
			memset( pixels, 0xff, p_size );
			p_w = w;
			p_h = h;
			p_bpp = F_4BPP;
			
			break;
	}
	
	prev = NULL;
	next = NULL;
	
}

FlopFrameClass::FlopFrameClass(FlopFrameClass* frame) {

	switch( frame->p_bpp ) {
		case F_4BPP:
			p_size = (frame->w()>>1)*frame->h();
			pixels = malloc( p_size );
			memset( pixels, 0xff, p_size );
			p_w = frame->w();
			p_h = frame->h();
			p_bpp = F_4BPP;
			break;
	}
	
	memcpy( pixels, frame->pixels, (frame->p_w>>1)*frame->p_h );
	
}

FlopFrameClass::~FlopFrameClass() {
	
	if( pixels )
		free( pixels );
	
}

int FlopFrameClass::ReadPixel(int x, int y) {
	
	if( x < 0 )
		return 0;
	if( y < 0 )
		return 0;
	if( x >= p_w )
		return 0;
	if( y >= p_h )
		return 0;
	
	switch( p_bpp ) {
		case F_4BPP:
			return (((unsigned char*)pixels)[(x>>1)+((p_w>>1)*y)]>>(4*(x&0x1)))&0xf;
	}
	
}

void FlopFrameClass::SetPixel(int x, int y, unsigned int col, int pattern, int pinv) {
	
	int t;
	
	if( x < 0 )
		return;
	if( y < 0 )
		return;
	if( x >= p_w )
		return;
	if( y >= p_h )
		return;
	
	switch( pattern ) {
	case 1:

		if( !pinv ) {
			
			if( ((x+y)%2) > 0 )
				return;
			
		} else {
			
			if( !(((x+y)%2) > 0) )
				return;

		}

		break;

	case 2:

		if( !pinv ) {

			if( ((x%2) > 0)||((y%2) > 0) )
				return;

		} else {

			if( !(((x%2) > 0)||((y%2) > 0)) )
				return;

		}
		break;

	}
	
	switch( p_bpp ) {
		case F_4BPP:
			if( ( x&0x1 ) == 0 ) {
				t = ((unsigned char*)pixels)[(x>>1)+((p_w>>1)*y)]&0xf0;
				t |= (col&0xf);
				((unsigned char*)pixels)[(x>>1)+((p_w>>1)*y)] = t;
			} else {
				t = ((unsigned char*)pixels)[(x>>1)+((p_w>>1)*y)]&0xf;
				t |= (col&0xf)<<4;
				((unsigned char*)pixels)[(x>>1)+((p_w>>1)*y)] = t;
			}
			break;
	}
	
}

void FlopFrameClass::SetPixelBrush(int x, int y, int shape, int size, unsigned int col, int pattern, int pinv) {

	int pixMapOffs = 0;
	int c = size>>1;
	int px,py;
	int i;

	if( shape == 0 ) {

		for( py=y; (py-y)<size; py++ ) {
			for( px=x; (px-x)<size; px++ ) {

				SetPixel( px-c, py-c, col, pattern, pinv );

			}
		}

	} else {

		for( i=1; i<size; i++ )
			pixMapOffs += 1+tbl_cmap[pixMapOffs];

		pixMapOffs++;

		for( py=y; (py-y)<size; py++ ) {
			for(px=x; (px-x)<size; px++) {

				if( tbl_cmap[pixMapOffs] )
					SetPixel( px-c, py-c, col, pattern, pinv );

				pixMapOffs++;

			}
		}

	}

}

void FlopFrameClass::SetLineBrush(int x1, int y1, int x2, int y2, int shape, int size, unsigned char col, int pattern, int pinv) {

	int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

	dx = x2-x1;      /* the horizontal distance of the line */
	dy = y2-y1;      /* the vertical distance of the line */
	dxabs = abs(dx);
	dyabs = abs(dy);
	sdx = sgn(dx);
	sdy = sgn(dy);
	x = dyabs>>1;
	y = dxabs>>1;
	px = x1;
	py = y1;

	SetPixelBrush(px, py, shape, size, col, pattern, pinv);

	if (dxabs >= dyabs) { /* the line is more horizontal than vertical */

		for(i=0; i<dxabs; i++) {

			y += dyabs;
			if (y >= dxabs) {

				y -= dxabs;
				py += sdy;
			}

			px += sdx;

			SetPixelBrush(px, py, shape, size, col, pattern, pinv);

		}

	} else { /* the line is more vertical than horizontal */

		for(i=0; i<dyabs; i++) {

			x += dxabs;

			if (x >= dyabs) {
				x -= dyabs;
				px += sdx;
			}

			py += sdy;

			SetPixelBrush(px, py, shape, size, col, pattern, pinv);

		}

	}

}

void FlopFrameClass::PasteTo(FlopFrameClass *frame) {
	
	for( int py=0; py<frame->p_h; py++ ) {
		for( int px=0; px<frame->p_w; px++ ) {
			
			frame->SetPixel( px, py, 
				ReadPixel( px, py ) );
			
		}
	}
	
}