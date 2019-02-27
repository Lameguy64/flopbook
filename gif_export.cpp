/* 
 * File:   gif_export.cpp
 * Author: Lameguy64
 * 
 * Created on February 18, 2019, 9:00 AM
 */

#include <string.h>
#include <math.h>
#include <string>
#ifdef DEBUG
#include <iostream>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <FreeImage.h>
#include <Fl/Fl_Native_File_Chooser.H>
#include <Fl/fl_message.H>

#include "mainui.h"
#include "FlopProjectClass.h"


extern MainUI *ui;
extern FlopProjectClass project;

extern int file_modified;
void cb_SaveProject(Fl_Menu_* w, long type);

void cb_ExportGIF(Fl_Menu_ *w, void *u) {
	
	if( !project.canvas ) {
		fl_message_title( "No Animation" );
		fl_message( "You cannot export a GIF with no animation loaded." );
		return;
	}
	
	if( file_modified ) {
		
		fl_message_title( "Document Modified" );
		int res = fl_choice( "Current document is not yet saved. Save it now?", "Cancel", "Yes", "No" );
		
		if( res == 0 )
			return;
		
		if( res == 1 ) {
			
			cb_SaveProject( (Fl_Menu_*)w, 0 );
			
			if( file_modified )
				return;
			
		}
		
	}
	
	Fl_Native_File_Chooser chooser;

	chooser.title( "Export GIF As" );
	chooser.filter( "GIF Image\t*.gif" );
	chooser.type( chooser.BROWSE_SAVE_FILE );
	chooser.options( chooser.SAVEAS_CONFIRM );

	if( chooser.show() != 0 )
		return;
	
	std::string gif_file = chooser.filename();
	
	if( gif_file.find( '.' ) == std::string::npos )
		gif_file.append( ".gif" );
	
	// Start GIF export process
	
	FreeImage_Initialise();
	
	FIMULTIBITMAP *gif_seq;
	FIBITMAP *gif_frame;
	RGBQUAD *gif_pal;
	int gif_rate;
	
#ifdef DEBUG
	std::cout << "Exporting GIF as " << gif_file << std::endl;
#endif
	
	fl_message_title( "Failed exporting GIF" );
	
	gif_seq = FreeImage_OpenMultiBitmap( FIF_GIF, gif_file.c_str(), true, false, true );
	
	if( !gif_seq ) {
		fl_message( "Unable to export GIF image." );
		FreeImage_DeInitialise();
		return;
	}
	
	gif_frame	= FreeImage_Allocate( project.project_w, project.project_h, 4 );
	
	if( !gif_frame ) {
		fl_message( "Unable to allocate a frame." );
		FreeImage_CloseMultiBitmap( gif_seq );
		FreeImage_DeInitialise();
		return;
	}
	
	gif_pal		= FreeImage_GetPalette( gif_frame );
	
	if( !gif_pal ) {
		fl_message( "Unable to get image palette." );
		FreeImage_CloseMultiBitmap( gif_seq );
		FreeImage_Unload( gif_frame );
		FreeImage_DeInitialise();
		return;
	}
	
	for( int i=0; i<16; i++ ) {
		gif_pal[i].rgbRed	= project.palette[i].r;
		gif_pal[i].rgbGreen	= project.palette[i].g;
		gif_pal[i].rgbBlue	= project.palette[i].b;
	}
	
	if( ui->rateMode[0]->value() ) {
		gif_rate = floor(ui->rateValue->value());
	} else {
		gif_rate = ceil(1000.f/ui->rateValue->value())+15;
	}
	
	FITAG *tag = FreeImage_CreateTag();
	if( tag ) {
		FreeImage_SetTagKey(tag, "FrameTime");
		FreeImage_SetTagType(tag, FIDT_LONG);
		FreeImage_SetTagCount(tag, 1);
		FreeImage_SetTagLength(tag, 4);
		FreeImage_SetTagValue(tag, &gif_rate);
		FreeImage_SetMetadata(FIMD_ANIMATION, gif_frame, FreeImage_GetTagKey(tag), tag);
		FreeImage_DeleteTag(tag);
	}
	
	for( int i=0; i<project.frames.size(); i++ ) {
		
		for( int py=0; py<project.project_h; py++ ) {
			for( int px=0; px<project.project_w; px++ ) {
				
				int src_p = project.frames[i]->ReadPixel( px, py );
				
				FreeImage_SetPixelIndex( gif_frame, px, (project.project_h-1)-py, (BYTE*)&src_p );
				
			}
		}
		
		FreeImage_AppendPage( gif_seq, gif_frame );
		
	}
	
	FreeImage_Unload( gif_frame );
	FreeImage_CloseMultiBitmap( gif_seq, GIF_DEFAULT );
	
	FreeImage_DeInitialise();
	
	fl_message_title( "Export" );
	fl_message( "GIF exported successfully." );
		
}