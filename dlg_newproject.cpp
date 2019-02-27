/* 
 * File:   dlg_newproject.cpp
 * Author: Lameguy64
 * 
 * Created on February 14, 2019, 10:09 AM
 */

#include <Fl/fl_message.H>
#include <string>
#include "FlopProjectClass.h"
#include "mainui.h"
#include "global.h"

extern MainUI *ui;
extern FlopProjectClass project;

extern std::string file_name;
extern int file_modified;


static NewProjectUI *dialog;
static int is_okay = false;

void cb_SaveProject(Fl_Menu_ *w, long type);

void cb_NewProject(Fl_Menu_ *w, void *u) {
	
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
	
	dialog = new NewProjectUI();
	
	dialog->show();
	
	while( dialog->shown() ) {
		Fl::wait();
	}
	
	if( is_okay ) {
		
		ui->label( PROGRAM_TITLE );
		
		project.NewProject( 
			dialog->projectXres->value(),
			dialog->projectYres->value(),
			FlopFrameClass::F_4BPP );
		
		ui->frameCtrlGroup->activate();
		ui->frameCounter->maximum( 0 );
		ui->frameCounter->value( 0 );
		
		ui->zoomRadio[0]->set();
		ui->zoomRadio[1]->clear();
		ui->zoomRadio[2]->clear();
		ui->zoomRadio[3]->clear();
		
		file_name.clear();
		file_modified = false;
		
	}
	
	delete dialog;
	
}

void cb_NewProject_Okay(Fl_Return_Button *w, void *u) {
	
	is_okay = true;
	dialog->hide();
	
}

void cb_NewProject_Cancel(Fl_Button *w, void *u) {
	
	is_okay = false;
	dialog->hide();
	
}