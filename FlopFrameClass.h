/* 
 * File:   FlopFrameClass.h
 * Author: Lameguy64
 *
 * Created on February 14, 2019, 10:20 AM
 */

#ifndef FLOPFRAMECLASS_H
#define FLOPFRAMECLASS_H

class FlopFrameClass {
public:
	
	enum {
		F_4BPP = 0,
		F_8BPP,
		F_32BPP
	} COLOR_DEPTH;
	
	FlopFrameClass(int w, int h, int bpp);
	FlopFrameClass(FlopFrameClass *frame);
	
	virtual ~FlopFrameClass();
	
	void PasteTo(FlopFrameClass *frame);
	
	int ReadPixel(int x, int y);
	void SetPixel(int x, int y, unsigned int col, int pattern = 0, int pinv = 0);
	
	void SetPixelBrush(int x, int y, int shape, int size, unsigned int col, int pattern = 0, int pinv = 0);
	void SetLineBrush(int x1, int y1, int x2, int y2, int shape, int size, unsigned char col, int pattern = 0, int pinv = 0);
	
	int w() {
		return p_w;
	}
	
	int h() {
		return p_h;
	}
	
	FlopFrameClass *prev;
	FlopFrameClass *next;
	
	void *data() {
		return pixels;
	}
	
	int dsize() {
		return p_size;
	}
	
private:
	
	void *pixels;
	int p_w,p_h;
	int p_size;
	int p_bpp;
	
};

#endif /* FLOPFRAMECLASS_H */

