/* 
 * File:   FlopProjectClass.cpp
 * Author: Lameguy64
 * 
 * Created on February 14, 2019, 10:40 AM
 */

#ifdef DEBUG
#include <iostream>
#endif
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <zlib/zlib.h>
#else
#include <zlib.h>
#endif
#include "FlopProjectClass.h"
#include "mainui.h"

extern MainUI *ui;
void cb_CanvasCallback(Fl_CanvasClass *w, void *u);

unsigned int palette_default[] = {
	0x00000000,
	0x00aa0000,
	0x0000aa00,
	0x0000aaaa,
	0x000000aa,
	0x00aa00aa,
	0x000055aa,
	0x00aaaaaa,
	0x00555555,
	0x00ff5555,
	0x0055ff55,
	0x00ffff55,
	0x005555ff,
	0x00ff55ff,
	0x0055ffff,
	0x00ffffff
};

FlopProjectClass::FlopProjectClass() {
	
	project_w = 0;
	project_h = 0;
	
	project_bpp = 0;
	
	canvas = NULL;
	palette = NULL;
	
	undo_index = -1;
	
}

FlopProjectClass::~FlopProjectClass() {
	
	if( frames.size() ) {
		
		for( int i=0; i<frames.size(); i++ )
			delete frames[i];
		
		frames.clear();
		
	}
	
	if( palette ) {
		free( palette );
		palette = NULL;
	}
	
	if( canvas ) {
		Fl::delete_widget( canvas );
		canvas = NULL;
	}
	
	for( int i=0; i<undo_list.size(); i++ ) {
		
		if( undo_list[i].frame )
			delete undo_list[i].frame;
		
	}
	
}

void FlopProjectClass::ClearProject() {
	
	if( frames.size() ) {
		
		for( int i=0; i<frames.size(); i++ )
			delete frames[i];
		
		frames.clear();
		
	}
	
	if( palette ) {
		free( palette );
		palette = NULL;
	}
	
	project_w = 0;
	project_h = 0;
	project_bpp = 0;
	
	if( canvas ) {
		Fl::delete_widget( canvas );
		canvas = NULL;
	}
	
	for( int i=0; i<undo_list.size(); i++ ) {
		
		if( undo_list[i].frame )
			delete undo_list[i].frame;
		
	}
	
	undo_list.clear();
	undo_index = -1;
	
}

void FlopProjectClass::NewProject(int w, int h, int bpp) {
	
	ClearProject();
	
	project_w = w;
	project_h = h;
	project_bpp = bpp;
	
	
	FlopFrameClass* f = new FlopFrameClass( w, h, bpp );
	frames.push_back( f );
	
	canvas = new Fl_CanvasClass( 
		ui->canvasArea->x()+32, ui->canvasArea->y()+32,
		w+5, h+5 );
	
	canvas->SetFrame( frames[0] );
	canvas->callback( (Fl_Callback_p)cb_CanvasCallback );
	
	ui->canvasArea->add( canvas );
	
	palette = (PAL_COLOR*)malloc( sizeof(PAL_COLOR)*16 );
	memcpy( palette, palette_default, sizeof(unsigned int)*16 );
	
	canvas->redraw();
	ui->paletteBox->SetPalette( palette );
	
}

int FlopProjectClass::SaveProject(const char* filename, int complevel) {
	
	FILE *fp = fopen( filename, "wb" );
	
	if( !fp ) {
		return 1;
	}
	
	// Write header
	FILE_HEADER head;
	
	memset( &head, 0x0, sizeof(FILE_HEADER) );
	
	memcpy( head.id, "FLoP", 4 );
	
	head.flags.rmode = ui->rateMode[1]->value();
	head.rate = ui->rateValue->value();
	head.flags.bpp = project_bpp;
	head.xres = project_w;
	head.yres = project_h;
	
	head.frames = frames.size();
	
	fwrite( &head, 1, sizeof(FILE_HEADER), fp );
	
	// Write palette data
	fwrite( palette, sizeof(PAL_COLOR), 16, fp );
	
	for( int i=0; i<frames.size(); i++ ) {
		
		FILE_FHEADER fhead;
		void *cbuff;
		uLongf cbufflen;
		
		fhead.crc32 = crc32( 0, Z_NULL, 0 );
		fhead.crc32 = crc32( fhead.crc32, (Bytef*)frames[i]->data(), frames[i]->dsize() );
		
		cbufflen = compressBound( frames[i]->dsize() );
		cbuff = malloc( cbufflen );
		if( compress2( (Bytef*)cbuff, &cbufflen, (const Bytef*)frames[i]->data(), frames[i]->dsize(), complevel ) != Z_OK ) {
			free( cbuff );
			fclose( fp );
			return 2;
		}
		
		fhead.csize = cbufflen;
		fwrite( &fhead, 1, sizeof(FILE_FHEADER), fp );
		fwrite( cbuff, 1, cbufflen, fp );
		
		free( cbuff );
		
	}
	
	fclose( fp );
	
	return 0;
	
}

int FlopProjectClass::LoadProject(const char* filename) {
	
	int r;
	FILE *fp = fopen( filename, "rb" );
	
	if( !fp ) {
		return 1;
	}
	
	FILE_HEADER head;
	
	fread( &head, 1, sizeof(FILE_HEADER), fp );
	
	if( strncmp( head.id, "FLoP", 4 ) ) {
		fclose( fp );
		return 2;
	}
	
	if( head.version > 0 ) {
		fclose( fp );
		return 3;
	}
	
	if( head.flags.bpp > 0 ) {
		fclose( fp );
		return 4;
	}
	
	ClearProject();
	
	project_w = head.xres;
	project_h = head.yres;
	project_bpp = head.flags.bpp;
	
	palette = (PAL_COLOR*)malloc( sizeof(PAL_COLOR)*16 );
	fread( palette, sizeof(PAL_COLOR), 16, fp );
	
	
	for( int i=0; i<head.frames; i++ ) {
		
		FILE_FHEADER fhead;
		Bytef *cbuff;
		uLongf cbufflen;
				
		fread( &fhead, 1, sizeof(FILE_FHEADER), fp );
		
		if( i == 0 ) {
			FlopFrameClass* f = new FlopFrameClass( project_w, project_h, project_bpp );
			frames.push_back( f );
		} else {
			NewFrame( -1, true );
		}
		
		cbuff = (Bytef*)malloc( fhead.csize );
		
		fread( cbuff, 1, fhead.csize, fp );
		
#ifdef DEBUG
		std::cout << "f.csize = " << fhead.csize << std::endl;
#endif

		cbufflen = frames[i]->dsize();
		r = uncompress( (Bytef*)frames[i]->data(), &cbufflen, cbuff, fhead.csize );
#ifdef DEBUG
		if( r != Z_OK ) {

			std::cout << "Uncompress error = " << r << std::endl;
		}
#endif
		free( cbuff );
		
	}
	
	fclose( fp );
	
	canvas = new Fl_CanvasClass( 
		ui->canvasArea->x()+32, ui->canvasArea->y()+32,
		project_w+5, project_h+5 );
	
	canvas->SetFrame( frames[0] );
	canvas->callback( (Fl_Callback_p)cb_CanvasCallback );
	ui->canvasArea->add( canvas );
	
	canvas->redraw();
	ui->paletteBox->SetPalette( palette );
	
	ui->frameCounter->value( 0 );
	ui->frameCounter->maximum( frames.size()-1 ); 
	ui->frameCtrlGroup->activate();
	
	ui->rateValue->value( head.rate );
	
	if( head.flags.rmode == 0 ) {
		ui->rateMode[0]->value( 1 );
		ui->rateMode[1]->value( 0 );
	} else {
		ui->rateMode[0]->value( 0 );
		ui->rateMode[1]->value( 1 );
	}
	
	return 0;
	
}

int FlopProjectClass::NewFrame(int fnum, int no_undo) {
	
#ifdef DEBUG
		std::cout << "Function called." << std::endl;
#endif
		
	if( fnum < 0 )	
		fnum = frames.size()-1;
	
	FlopFrameClass *frame;
	
	if( fnum >= (frames.size()-1) ) {

		frame = new FlopFrameClass( project_w, project_h, project_bpp );

		frames[frames.size()-1]->next = frame;
		frame->prev = frames[frames.size()-1];
		
		frames.push_back( frame );
		
#ifdef DEBUG
		std::cout << "Created frame." << std::endl;
#endif
		
		if( !no_undo ) {
			
			/*if( undo_index < undo_list.size()-1 ) {

				for( int i=undo_index; i<undo_list.size(); i++ ) {
					if( undo_list[i].frame )
						delete undo_list[i].frame;
				}

				undo_list.erase( undo_list.begin()+undo_index, undo_list.end() );

			}*/
			
			UndoEntryType undo;

			undo.frame		= NULL;
			undo.frame_num	= frames.size()-1;
			undo.type		= UNDO_NEW;

			undo_list.push_back( undo );
			undo_index = undo_list.size()-1;
#ifdef DEBUG
			std::cout << "Undo index = " << undo_index << std::endl;
#endif
			
		}
	
		return frames.size()-1;

	} else {

		frames.push_back( NULL );
		fnum++;
		
		frame = new FlopFrameClass( project_w, project_h, project_bpp );
		
		frame->prev = frames[fnum-1];
		frame->next = frames[fnum];
		
		frames[fnum-1]->next = frame;
		frames[fnum]->prev = frame;
		
		for( int i=frames.size()-1; i>fnum; i-- ) {
			frames[i] = frames[i-1];
		}
		
		frames[fnum] = frame;
		
#ifdef DEBUG
		std::cout << "Inserted frame." << std::endl;
#endif
		
		if( !no_undo ) {
			
			/*if( undo_index < undo_list.size()-1 ) {
			
				for( int i=undo_index; i<undo_list.size(); i++ ) {
					if( undo_list[i].frame )
						delete undo_list[i].frame;
				}

				undo_list.erase( undo_list.begin()+undo_index, undo_list.end() );

			}*/
			
			UndoEntryType undo;
		
			undo.frame		= NULL;
			undo.frame_num	= fnum;
			undo.type		= UNDO_NEW;

			undo_list.push_back( undo );
			undo_index = undo_list.size()-1;
#ifdef DEBUG
			std::cout << "Undo index = " << undo_index << std::endl;
#endif
		}
		
		return fnum;
		
	}
	
}

void FlopProjectClass::DeleteFrame(int fnum, int no_undo) {
	
	if( frames.size() <= 1 )
		return;
	
	int i;
	
	if( fnum <= 0 ) {
		frames[fnum+1]->prev = NULL;
	} else if( fnum >= frames.size()-1 ) {
		frames[fnum-1]->next = NULL;
	} else {
		frames[fnum-1]->next = frames[fnum+1];
		frames[fnum+1]->prev = frames[fnum-1];
	}
	
	if( !no_undo ) {
		
		/*if( undo_index < undo_list.size()-1 ) {
			
			for( int i=undo_index; i<undo_list.size(); i++ ) {
				if( undo_list[i].frame )
					delete undo_list[i].frame;
			}
			
			undo_list.erase( undo_list.begin()+undo_index, undo_list.end() );
			
		}*/
		
		UndoEntryType undo;

		undo.frame		= new FlopFrameClass( frames[fnum] );
		undo.frame_num	= fnum;
		undo.type		= UNDO_DELETE;

		undo_list.push_back( undo );
		undo_index = undo_list.size()-1;
		
#ifdef DEBUG
		std::cout << "Undo index = " << undo_index << std::endl;
#endif
		
	}
	
	delete frames[fnum];
	frames.erase( frames.begin()+fnum );
	
}

int FlopProjectClass::FindFrameIndex(FlopFrameClass* frame) {
	
	for( int i=0; i<frames.size(); i++ ) {
		if( frame == frames[i] )
			return i;
	}
	
	return -1;
	
}

void FlopProjectClass::PushUndo(int fnum) {
	
	if( fnum < 0 )
		return;
	if( fnum > frames.size()-1 )
		return;
	
	/*if( undo_index < undo_list.size()-1 ) {
			
		for( int i=undo_index; i<undo_list.size(); i++ ) {
			if( undo_list[i].frame )
				delete undo_list[i].frame;
		}

		undo_list.erase( undo_list.begin()+undo_index, undo_list.end() );

	}*/
	
	UndoEntryType undo;
	
	undo.frame		= new FlopFrameClass( frames[fnum] );
	undo.frame_num	= fnum;
	undo.type		= UNDO_DRAW;
	
	undo_list.push_back( undo );
	undo_index = undo_list.size()-1;
	
#ifdef DEBUG
	std::cout << "Undo index = " << undo_index << std::endl;
#endif
	
}

int FlopProjectClass::Undo() {
	
	if( !undo_list.size() )
		return -1;
	
	if( undo_index < 0 )
		return -1;
	
	int fnum = undo_list[undo_index].frame_num;
	
	switch( undo_list[undo_index].type ) {
	case FlopProjectClass::UNDO_DRAW:
		undo_list[undo_index].frame->PasteTo( frames[fnum] );
#ifdef DEBUG
		std::cout << "Undo index W = " << undo_index << std::endl;
#endif
		delete undo_list[undo_index].frame;
		undo_list.pop_back();
		undo_index--;
		break;
	case FlopProjectClass::UNDO_NEW:
		DeleteFrame( fnum, true );
		undo_list.pop_back();
		undo_index--;
#ifdef DEBUG
		std::cout << "Undo index N = " << undo_index << std::endl;
#endif
		fnum--;
		if( fnum < 0 )
			return 0;
		break;
	case FlopProjectClass::UNDO_DELETE:
		NewFrame( fnum-1, true );
		undo_list[undo_index].frame->PasteTo( frames[fnum] );
		delete undo_list[undo_index].frame;
		undo_list.pop_back();
		undo_index--;
#ifdef DEBUG
		std::cout << "Undo index D = " << undo_index << std::endl;
#endif
		break;
	}
	
	return fnum;
	
}