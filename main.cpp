/* 
 * File:   main.cpp
 * Author: Lameguy64
 *
 * Created on February 14, 2019, 8:53 AM
 */

#include <string>
#ifdef DEBUG
#include <iostream>
#endif

#include <Fl/Fl_Color_Chooser.H>
#include <Fl/Fl_Native_File_Chooser.H>
#include <Fl/Fl_RGB_Image.H>
#include <Fl/fl_message.H>

#include "global.h"
#include "mainui.h"
#include "FlopProjectClass.h"
#include "Fl_ColorPalette.h"
#include "icons/icon.c"

MainUI *ui;
FlopProjectClass project;
FlopFrameClass *clipboard = NULL;


int file_modified = false;
std::string file_name;

int tool_color_l = 0;
int tool_color_r = 15;
int tool_erase_col;
int tool_ref_frame = 0;
int tool_mode = 0;

int play_fnum;
int play_stop;


int main(int argc, char** argv) {

	ui = new MainUI();
	
	ui->label( PROGRAM_TITLE );
	
	Fl_RGB_Image *icon = new Fl_RGB_Image( app_icon.pixel_data, 
		app_icon.width, app_icon.height, app_icon.bytes_per_pixel );
			
	ui->icon( icon );
	
	ui->show();
	
	if( argc > 1 ) {
		
		fl_message_title( "Error Loading File" );
		switch( project.LoadProject( argv[1] ) ) {
			case 1:
				fl_message( "File not found." );
				break;
			case 2:
				fl_message( "File is not a Flopbook document." );
				break;
			case 3:
			case 4:
				fl_message( "Flopbook document not supported in this version." );
				break;
		}
		
		file_name = argv[1];
		file_modified = false;
		
	}
	
	int ret = Fl::run();
	delete icon;
	return ret;
	
}

void UpdateTitle() {

	std::string temp = file_name;

	std::size_t it = temp.find_last_of( "\\" );

	if( it == std::string::npos ) {
		it = temp.find_last_of( "//" );
	}

	if( it != std::string::npos ) {
		temp.erase( temp.begin(), temp.begin()+it+1 );
	}

	temp = PROGRAM_TITLE " (" + temp + ")";

	ui->label( temp.c_str() );

}

void cb_PlayAnimFunc(void *u);
void cb_SaveProject(Fl_Menu_ *w, long type);

void cb_CloseCallback(Fl_Double_Window *w, long u) {
	
	Fl::remove_timeout( cb_PlayAnimFunc );
	
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
	
	w->hide();
	
}

void cb_CloseMenu(Fl_Menu_ *w, void *u) {
	
	cb_CloseCallback( ui, 0 );
	
}

void cb_SaveProject(Fl_Menu_ *w, long type) {
	
	if( !project.canvas ) {
		
		fl_message_title( "No Project Present" );
		fl_message( "Cannot save an empty project." );
		return;
		
	}
	
	if( ( file_name.empty() ) || ( type ) ) {
		
		Fl_Native_File_Chooser chooser;

		chooser.title( "Save Animation As" );
		chooser.filter( "Flopbook Animation\t*.fba" );
		chooser.type( chooser.BROWSE_SAVE_FILE );
		chooser.options( chooser.SAVEAS_CONFIRM );

		if( chooser.show() == 0 ) {
#ifdef DEBUG
			std::cout << "Save as " << chooser.filename() << std::endl;
#endif
			file_name = chooser.filename();

			if( file_name.find( '.' ) == std::string::npos )
				file_name.append( ".fba" );

			fl_message_title( "Error Saving File" );
			switch( project.SaveProject( file_name.c_str(), 9 ) ) {
				case 1:
					fl_message( "Cannot create file." );
					return;
				case 2:
					fl_message( "Data compression error." );
					return;
			}
			
			UpdateTitle();
			
			file_modified = false;

		} else {
			
			return;
			
		}
		
	} else {
		
		switch( project.SaveProject( file_name.c_str(), 9 ) ) {
			case 1:
				fl_message( "Cannot create file." );
				return;
			case 2:
				fl_message( "Data compression error." );
				return;
		}

		file_modified = false;
		
	}
	
}

void cb_OpenProject(Fl_Menu_ *w, void *u) {
	
	Fl::remove_timeout( cb_PlayAnimFunc );
	
	if( file_modified ) {
		
		fl_message_title( "Document Modified" );
		int res = fl_choice( "Current document is not yet saved. Save it now?", "Cancel", "Yes", "No" );
		
		if( res == 0 )
			return;
		
		if( res == 1 ) {
			cb_SaveProject( w, 0 );
		}
		
	}
	
	Fl_Native_File_Chooser chooser;
	
	chooser.title( "Open Animation" );
	chooser.filter( "Flopbook Animation\t*.fba" );
	chooser.type( chooser.BROWSE_FILE );
	
	if( chooser.show() == 0 ) {
		
		fl_message_title( "Error Loading File" );
		switch( project.LoadProject( chooser.filename() ) ) {
			case 1:
				fl_message( "File not found." );
				break;
			case 2:
				fl_message( "File is not a Flopbook document." );
				break;
			case 3:
			case 4:
				fl_message( "Flopbook document not supported in this version." );
				break;
		}
		
		file_name = chooser.filename();
		file_modified = false;
		
		ui->zoomRadio[0]->set();
		ui->zoomRadio[1]->clear();
		ui->zoomRadio[2]->clear();
		ui->zoomRadio[3]->clear();
		
		UpdateTitle();
		
	}
	
	
}


void cb_Zoom(Fl_Button* w, long v) {
	
	if( !project.canvas )
		return;
	
	int last_zoom = project.canvas->Scale();
	
	project.canvas->position(
		(ui->canvasArea->x()+(ui->canvasArea->w()>>1))-((project.project_w<<(v))>>1),
		(ui->canvasArea->y()+(ui->canvasArea->h()>>1))-((project.project_h<<(v))>>1) );
	
	project.canvas->SetScale( v );
	
}

void cb_NewFrame(Fl_Button *w, void *u) {
	
	if( !project.canvas )
		return;
	
	int fnum = project.NewFrame( ui->frameCounter->value() );
	
	project.canvas->SetFrame( project.frames[fnum] );
	
	ui->frameCounter->maximum( project.frames.size()-1 );
	ui->frameCounter->value( fnum );
	file_modified = true;
	
}

void cb_NewFrameMenu(Fl_Menu_ *w, void *u) {
	cb_NewFrame( (Fl_Button*)w, u );
}


void cb_DeleteFrame(Fl_Button *w, void *u) {
	
	if( project.frames.size() <= 1 )
		return;
	
	project.DeleteFrame( ui->frameCounter->value() );
	
	ui->frameCounter->maximum( project.frames.size()-1 );
	if( ui->frameCounter->value() >= project.frames.size() ) {
		ui->frameCounter->value( project.frames.size()-1 );
	}
	
	project.canvas->SetFrame( project.frames[ui->frameCounter->value()] );
	file_modified = true;
	
}

void cb_DeleteFrameMenu(Fl_Menu_ *w, void *u) {
	cb_DeleteFrame( (Fl_Button*)w, u );
}


void cb_FrameCounter(Fl_Counter *w, void *u) {
	
	int fnum = w->value();
	
	project.canvas->SetFrame( project.frames[fnum] );
	
}

void cb_PlayAnimFunc(void *u) {
	
	play_fnum++;
	if( play_fnum >= project.frames.size() )
		play_fnum = 0;
	
	ui->frameCounter->value( play_fnum );
	project.canvas->SetFrame( project.frames[play_fnum] );
	
	if( ui->rateMode[0]->value() ) {
		Fl::repeat_timeout( ui->rateValue->value()/1000.f, cb_PlayAnimFunc );
	} else {
		Fl::repeat_timeout( 1.f/ui->rateValue->value(), cb_PlayAnimFunc );
	}
	
}

void cb_PlayAnim(Fl_Button *w, void *u) {
	
	play_fnum = 0;
	play_stop = 0;
	
	project.canvas->SetFrame( project.frames[0] );
	ui->frameCounter->value( play_fnum );
	
	Fl::remove_timeout( cb_PlayAnimFunc );
	if( ui->rateMode[0]->value() ) {
		Fl::add_timeout( ui->rateValue->value()/1000.f, cb_PlayAnimFunc );
	} else {
		Fl::add_timeout( 1.f/ui->rateValue->value(), cb_PlayAnimFunc );
	}
	
}

void cb_StopAnim(Fl_Button *w, void *u) {
	
	Fl::remove_timeout( cb_PlayAnimFunc );
	project.canvas->SetFrame( project.frames[ui->frameCounter->value()] );
	
}

void cb_PaletteBox(Fl_ColorPalette *w, void *u) {
	
	int index = ((Fl::event_x()-w->x())/20)+(8*((Fl::event_y()-w->y())/20));
	
	if( !project.palette )
		return;
	
	if( Fl::event_button1() ) {
		
		if( Fl::event_clicks() ) {
			
			if( fl_color_chooser( "Redefine Color", 
				project.palette[index].r,
				project.palette[index].g,
				project.palette[index].b ) ) {
				
				ui->paletteBox->redraw();
				project.canvas->redraw();
				file_modified = true;
				
			}
			
		}
		
		tool_color_l = index;
		ui->foreColor->color( fl_rgb_color( 
			project.palette[index].r, 
			project.palette[index].g, 
			project.palette[index].b ) );
		
		ui->colorSelection->redraw();
		
	} else if( Fl::event_button3() ) {
		
		tool_color_r = index;
		ui->backColor->color( fl_rgb_color( 
			project.palette[index].r, 
			project.palette[index].g, 
			project.palette[index].b ) );
		
		ui->colorSelection->redraw();
		
	}
	
}

void cb_OnionToggle(Fl_Button *w, void *u) {
	
	if( project.canvas ) {
		project.canvas->redraw();
	}
	
}

void cb_OnionRefToggle(Fl_Button *w, void *u) {
	
#ifdef DEBUG
	std::cout << "Reference frame set." << std::endl;
#endif
	
	tool_ref_frame = ui->frameCounter->value();
	
	if( project.canvas ) {
		project.canvas->redraw();
	}
	
}

void cb_GotoFirst(Fl_Menu_ *w, void *u) {
	
	if( !project.canvas )
		return;
	
	project.canvas->SetFrame( project.frames[0] );
	ui->frameCounter->value( 0 );
	
}

void cb_GotoLast(Fl_Menu_ *w, void *u) {

	if( !project.canvas )
		return;
	
	project.canvas->SetFrame( project.frames[project.frames.size()-1] );
	ui->frameCounter->value( project.frames.size()-1 );
	
}

void cb_ToolButton(Fl_Button *w, long u) {

	tool_mode = u;
	
	if( ( u == 1 ) && project.palette ) {
		
		int last_int=0;
		
		for( int i=0; i<16; i++ ) {
			
			if( ( project.palette[i].r+
				project.palette[i].g+
				project.palette[i].b ) > last_int ) {
				
				last_int = project.palette[i].r+
					project.palette[i].g+
					project.palette[i].b;
				
				tool_erase_col = i;
				
			}
			
		}
		
	}
	
}

void cb_CopyFrame(Fl_Menu_ *w, void *u) {
	
	if( !project.canvas )
		return;
	
	if( clipboard )
		delete clipboard;
	
	clipboard = new FlopFrameClass( project.frames[ui->frameCounter->value()] );
	
}

void cb_PasteFrame(Fl_Menu_ *w, void *u) {
	
	if( !project.canvas )
		return;
	
	if( !clipboard )
		return;
	
	clipboard->PasteTo( project.frames[ui->frameCounter->value()] );
	project.canvas->redraw();
	file_modified = true;
	
}

void cb_CanvasCallback(Fl_CanvasClass *w, void *u) {
	
	if( Fl::event_key( FL_Left ) ) {
		
		if( ui->frameCounter->value() > 0 ) {
			ui->frameCounter->value( ui->frameCounter->value()-1 );
			project.canvas->SetFrame( project.frames[ui->frameCounter->value()] );
		}
	
	} else if( Fl::event_key( FL_Right ) ) {
	
		if( ui->frameCounter->value() < project.frames.size()-1 ) {
			ui->frameCounter->value( ui->frameCounter->value()+1 );
			project.canvas->SetFrame( project.frames[ui->frameCounter->value()] );
		}
		
	}
	
}

void cb_Undo(Fl_Menu_ *w, void *u) {
	
	int fnum = project.Undo();
	
	if( fnum >= 0 ) {
		project.canvas->SetFrame( project.frames[fnum] );
		ui->frameCounter->value( fnum );
		ui->frameCounter->maximum( project.frames.size()-1 );
	}
	
}