/* 
 * File:   Fl_ColorPalette.cpp
 * Author: Lameguy64
 * 
 * Created on February 15, 2019, 12:59 PM
 */

#include <Fl/fl_draw.H>
#include "Fl_ColorPalette.h"

Fl_ColorPalette::Fl_ColorPalette(int x, int y, int w, int h, const char *l) 
	: Fl_Box( x, y, w, h, l ) {
	
	palette = NULL;
	
}

int Fl_ColorPalette::handle(int e) {
	
	switch( e ) {
		
		case FL_PUSH:
			
			if( callback() )
				callback()(this, user_data());
			
			return 1;
			
	}
	
	return Fl_Box::handle( e );
	
}

void Fl_ColorPalette::draw() {
	
	int px,py;
	
	fl_push_clip( x(), y(), w(), h() );
	
	for( py=0; py<2; py++ ) {
		for( px=0; px<8; px++ ) {
			
			if( palette ) {
				fl_draw_box( FL_DOWN_BOX, x()+20*px, y()+20*py, 20, 20,
					fl_rgb_color( palette[px+(8*py)].r, palette[px+(8*py)].g, palette[px+(8*py)].b ) );
			} else {
				fl_draw_box( FL_DOWN_BOX, x()+20*px, y()+20*py, 20, 20, FL_WHITE );
			}
			
		}
	}
	
	fl_pop_clip();
	
}