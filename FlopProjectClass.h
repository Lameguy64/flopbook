/* 
 * File:   FlopProjectClass.h
 * Author: Lameguy64
 *
 * Created on February 14, 2019, 10:40 AM
 */

#ifndef FLOPPROJECTCLASS_H
#define FLOPPROJECTCLASS_H

#include <Fl/Fl_Box.H>
#include <vector>

#include "Fl_CanvasClass.h"
#include "FlopFrameClass.h"

class FlopProjectClass {
public:

	enum {
		UNDO_DRAW,
		UNDO_NEW,
		UNDO_DELETE,
	} UndoType;
	
	typedef struct {
		FlopFrameClass	*frame;
		int				frame_num;
		int				type;
	} UndoEntryType;
	
	typedef struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char p;
	} PAL_COLOR;
	
	typedef struct {
		char	id[4];		// FLoP
		int		version;	// 0x00000000
		struct {
			unsigned int bpp:2;		// Bits per pixel (0-4bpp, 1-8bpp, 2-16bpp)
			unsigned int rmode:1;	// Framerate mode (0-ms, 1-fps)
			unsigned int res:19;	// Reserved space
		} flags;
		int		frames;		// Number of frames
		short	xres;		// X resolution of image
		short	yres;		// Y resolution of image
		short	layers;		// Number of layers per frame (future expansion))
		short	rate;		// Animation rate (type depends on flags.rmode)
	} FILE_HEADER;
	
	typedef struct {
		unsigned int csize;	// Size of compressed chunk
		int		crc32;		// CRC32 of decompressed frame data
	} FILE_FHEADER;
	
	FlopProjectClass();
	virtual ~FlopProjectClass();
	
	void ClearProject();
	void NewProject(int w, int h, int bpp);
	int SaveProject(const char* filename, int complevel);
	int LoadProject(const char* filename);
	
	int NewFrame(int fnum = -1, int no_undo = false);
	void DeleteFrame(int fnum, int no_undo = false);
	
	int FindFrameIndex(FlopFrameClass *frame);
	void PushUndo(int fnum);
	
	int Undo();
	
	int project_w;
	int project_h;
	int project_bpp;
	
	std::vector<UndoEntryType> undo_list;
	int undo_index;
	
	std::vector<FlopFrameClass*> frames;
	
	Fl_CanvasClass* canvas;
	PAL_COLOR *palette;
	
};

#endif /* FLOPPROJECTCLASS_H */

