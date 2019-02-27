/* 
 * File:   Fl_ColorPalette.h
 * Author: Lameguy64
 *
 * Created on February 15, 2019, 12:59 PM
 */

#ifndef FL_COLORPALETTE_H
#define FL_COLORPALETTE_H

#include <Fl/Fl_Box.H>
#include "FlopProjectClass.h"

class Fl_ColorPalette : public Fl_Box {
private:
	
	FlopProjectClass::PAL_COLOR *palette;
	
public:
	
	Fl_ColorPalette(int x, int y, int w, int h, const char *l = 0);

	void SetPalette(FlopProjectClass::PAL_COLOR *pal) {
		
		palette = pal;
		redraw();
		
	}
	
protected:
	
	int handle(int e);
	void draw();
	
};

#endif /* FL_COLORPALETTE_H */

