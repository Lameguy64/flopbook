/* 
 * File:   Fl_CanvasClass.h
 * Author: Lameguy64
 *
 * Created on February 14, 2019, 11:41 AM
 */

#ifndef FL_CANVASCLASS_H
#define FL_CANVASCLASS_H

#include <Fl/Fl_Box.H>
#include "FlopFrameClass.h"

class Fl_CanvasClass : public Fl_Box {
public:
	
	static void draw_func(void* userdata, int p_x, int p_y, int p_w, unsigned char *p);
	
	Fl_CanvasClass(int x, int y, int w, int h, const char* l = 0);
	virtual ~Fl_CanvasClass();
	
	void SetFrame(FlopFrameClass *f) {
		frame = f;
		redraw();
	}
	
	FlopFrameClass *Frame() {
		return frame;
	}
	
	void SetScale(int sc);
	int Scale() {
		return scale;
	}
	
protected:
	
	int handle(int e);
	void draw();
	
private:

	FlopFrameClass *frame;
	int scale;
	
	int drawing;
	int drag;
	
	int o_x,o_y;
	int of_x,of_y;
	
	int last_x,last_y;
	
};

#endif /* FL_CANVASCLASS_H */

