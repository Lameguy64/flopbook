/* 
 * File:   Fl_CanvasClass.cpp
 * Author: Lameguy64
 * 
 * Created on February 14, 2019, 11:41 AM
 */

#ifdef DEBUG
#include <iostream>
#endif

#include <Fl/Fl.H>
#include <Fl/Fl_Scroll.H>
#include <Fl/fl_draw.H>
#include "Fl_CanvasClass.h"

#include "mainui.h"
#include "FlopProjectClass.h"

extern MainUI *ui;

extern int file_modified;

extern int tool_ref_frame;
extern int tool_color_l;
extern int tool_color_r;
extern int tool_erase_col;
extern int tool_mode;

extern FlopProjectClass project;


typedef struct {
	short x,y;
} FillEntry;

std::vector<FillEntry> fillStack;

int PatternCheck(int x, int y, int pattern, int pinv) {
	
	switch( pattern ) {
	case 1:

		if( !pinv ) {
			
			if( ((x+y)%2) > 0 )
				return 0;
			
		} else {
			
			if( !(((x+y)%2) > 0) )
				return 0;

		}

		break;

	case 2:

		if( !pinv ) {

			if( ((x%2) > 0)||((y%2) > 0) )
				return 0;

		} else {

			if( !(((x%2) > 0)||((y%2) > 0)) )
				return 0;

		}
		break;

	}
	
	return 1;
	
}

int paintFillPop(int& x, int& y) {

	if( fillStack.size() > 0 ) {

		x = fillStack[fillStack.size()-1].x;
		y = fillStack[fillStack.size()-1].y;
		fillStack.pop_back();
		
		return 1;

	} else {

		return 0;

	}

}

int paintFillPush(int x, int y) {

	FillEntry f;
	f.x = x;
	f.y = y;
	
	fillStack.push_back( f );

	return 1;

}

void paintFill(int x, int y, int col, FlopFrameClass *frame) {

	FlopFrameClass *frameMask = frame;
			
	if( frame->ReadPixel( x, y ) == col)
		return;

	fillStack.clear();

	int oldCol = frame->ReadPixel( x, y );

	if( !paintFillPush( x, y ) )
		return;

	if( ui->patternChoice->value() > 0 )
		frameMask = new FlopFrameClass( frame );
	
	while( paintFillPop( x, y ) ) {

		int nx = x;
		int tcol[2] = { false, false };

		if( y > 0 ) {
			
			if( frameMask->ReadPixel( x, y-1 ) == oldCol )
				paintFillPush( x, y-1 );
			else
				tcol[0] = true;
			
		}
		
		if( y < frame->h()-1 ) {
		
			if( frameMask->ReadPixel( x, y+1 ) == oldCol )
				paintFillPush( x, y+1 );
			else
				tcol[1] = true;
		
		}

		// Scan and fill leftward
		while( frameMask->ReadPixel( nx, y ) == oldCol ) {

			if( tcol[0] >= 0 ) {

				if( y > 0 ) {
					if( frameMask->ReadPixel(nx, y-1) != oldCol ) {

						tcol[0] = true;

					} else if( ( frameMask->ReadPixel( nx, y-1 ) == oldCol ) && ( tcol[0] ) ) {

						paintFillPush( nx, y-1 );
						tcol[0] = false;

					}
				}

				if( y < frame->h()-1 ) {
					if( frameMask->ReadPixel(nx, y+1) != oldCol ) {

						tcol[1] = true;

					} else if( ( frameMask->ReadPixel( nx, y+1 ) == oldCol ) && ( tcol[1] ) ) {

						paintFillPush( nx, y+1 );
						tcol[1] = false;

					}
				}

			}

			frameMask->SetPixel( nx, y, col );
			if( ui->patternChoice->value() ) {
				
				if( PatternCheck( nx, y, ui->patternChoice->value(), 
					ui->patternInvert->value() ) )
					frame->SetPixel(nx, y, col);
				
			}
			nx--;
			
			if( nx < 0 )
				break;

		}

		tcol[0] = false;
		tcol[1] = false;

		if( y > 0 ) {
			
			if( frameMask->ReadPixel( x, y-1 ) != oldCol )
				tcol[0] = true;
		
		}
		
		if( y < frameMask->h() ) {
			
			if( frameMask->ReadPixel( x, y+1 ) != oldCol )
				tcol[1] = true;
			
		}

		nx = x+1;

		// Scan rightward
		while( frameMask->ReadPixel( nx, y ) == oldCol ) {

			if( tcol[0] >= 0 ) {

				if( y > 0 ) {
					if( frameMask->ReadPixel( nx, y-1 ) != oldCol ) {

						tcol[0] = true;

					} else if( ( frameMask->ReadPixel( nx, y-1 ) == oldCol) && ( tcol[0] ) ) {

						paintFillPush(nx, y-1);
						tcol[0] = false;

					}
				}

				if( y < frame->h()-1 ) {
					if( frameMask->ReadPixel( nx, y+1 ) != oldCol ) {

						tcol[1] = true;

					} else if( ( frameMask->ReadPixel( nx, y+1 ) == oldCol ) && ( tcol[1] ) ) {

						paintFillPush( nx, y+1 );
						tcol[1] = false;

					}
				}

			}

			frameMask->SetPixel(nx, y, col);
			if( ui->patternChoice->value() ) {
				
				if( PatternCheck( nx, y , ui->patternChoice->value(), 
					ui->patternInvert->value() ) )
					frame->SetPixel(nx, y, col);
				
			}
			nx++;

			if( nx >= frame->w() )
				break;
			
		}

	}
	
	fillStack.clear();
	
	if( ui->patternChoice->value() > 0 )
		delete frameMask;

}


Fl_CanvasClass::Fl_CanvasClass(int x, int y, int w, int h, const char *l)
	: Fl_Box( x, y, w, h, l ) {
	
	box( FL_SHADOW_BOX );
	color( FL_WHITE );
	
	frame = NULL;
	scale = 0;
	
	drawing = false;
	
}

Fl_CanvasClass::~Fl_CanvasClass() {
}

int Fl_CanvasClass::handle(int e) {
	
	int c;
	
	switch( e ) {
		case FL_ENTER:
			Fl::focus( this );
			fl_cursor( FL_CURSOR_CROSS );
			break;
			
		case FL_LEAVE:
			fl_cursor( FL_CURSOR_ARROW );
			break;
			
		case FL_KEYBOARD:
			if( ( Fl::event_key() == FL_Left ) || ( Fl::event_key() == FL_Right ) ) {
				if( callback() )
					callback()( this, user_data() );
				return 1;
			}
			break;
			
		case FL_PUSH:
			
			Fl::focus( this );
			
			if( ( Fl::event_button1() || Fl::event_button3() ) && ( !drag ) ) {
				
				if( tool_mode == 1 ) {
					
					c = tool_erase_col;
					
				} else {
					
					if( Fl::event_button1() ) {

						c = tool_color_l;

					} else if( Fl::event_button3() ) {

						c = tool_color_r;

					}
					
				}
				
				if( tool_mode < 2 ) {
					
					project.PushUndo( ui->frameCounter->value() );
					
					frame->SetPixelBrush(
						((Fl::event_x()-x())-1)>>scale,
						((Fl::event_y()-y())-1)>>scale,
						ui->brushShape[0]->value(), 
						ui->brushSize->value(), c,
						ui->patternChoice->value(),
						ui->patternInvert->value() );

					file_modified = true;

					last_x = ((Fl::event_x()-x())-1)>>scale;
					last_y = ((Fl::event_y()-y())-1)>>scale;

					drawing = true;
					redraw();
					return 1;
					
				} else if( tool_mode == 2 ) {
					
					project.PushUndo( ui->frameCounter->value() );
					
					paintFill( 
						((Fl::event_x()-x())-1)>>scale,
						((Fl::event_y()-y())-1)>>scale,
						c, frame );
					
					redraw();
					
				}
				
			} else if( Fl::event_button2() ) {
				
				o_x = x();
				o_y = y();
				of_x = Fl::event_x()-x();
				of_y = Fl::event_y()-y();
				drag = true;
				return 1;
				
			}
			
			break;
			
		case FL_DRAG:
			
			if( drawing ) {
				
				if( tool_mode == 0 ) {
					
					if( Fl::event_button1() ) {

						c = tool_color_l;

					} else if( Fl::event_button3() ) {

						c = tool_color_r;

					}
					
				} else if( tool_mode == 1 ) {
					
					c = tool_erase_col;
					
				}
				
				frame->SetLineBrush(
					last_x, last_y,
					((Fl::event_x()-x())-1)>>scale,
					((Fl::event_y()-y())-1)>>scale,
					ui->brushShape[0]->value(), 
					ui->brushSize->value(), c,
					ui->patternChoice->value(),
					ui->patternInvert->value() );
				
				last_x = ((Fl::event_x()-x())-1)>>scale;
				last_y = ((Fl::event_y()-y())-1)>>scale;
				
				redraw();
				return 1;
			}
			
			if( drag ) {
				
				((Fl_Scroll*)parent())->scroll_to(
					-((Fl::event_x()-of_x)-parent()->x()-2),
					-((Fl::event_y()-of_y)-parent()->y()-2) );
				
				
				return 1;
			}
			
			break;
			
		case FL_RELEASE:
			
			drawing = false;
			drag = false;
			
			break;
	}
	
	return Fl_Box::handle( e );
	
}

void Fl_CanvasClass::draw_func(void* userdata, int p_x, int p_y, int p_w, unsigned char* p) {
	
	Fl_CanvasClass *canvas = (Fl_CanvasClass*)userdata;
	int c;
	int ll,rr,gg,bb;
	
	for( int xx = p_x; xx<p_x+(p_w); xx++ ) {
		
		if( ui->onionPrevToggle->value() || ui->onionNextToggle->value() 
			|| ui->onionRefToggle->value() ) {
			
			ll = 0; rr = 0; gg = 0; bb = 0;

			if( ( canvas->frame->prev ) && ( ui->onionPrevToggle->value() ) ) {
				c = canvas->frame->prev->ReadPixel( xx>>canvas->scale, p_y>>canvas->scale );
				rr += project.palette[c].r;
				gg += project.palette[c].g;
				bb += project.palette[c].b;
				ll++;
			}

			if( ( canvas->frame->next ) && ( ui->onionNextToggle->value() ) ) {
				c = canvas->frame->next->ReadPixel( xx>>canvas->scale, p_y>>canvas->scale );
				rr += project.palette[c].r;
				gg += project.palette[c].g;
				bb += project.palette[c].b;
				ll++;
			}
			
			if( ui->onionRefToggle->value() ) {
				c = project.frames[tool_ref_frame]->ReadPixel( xx>>canvas->scale, p_y>>canvas->scale );
				rr += project.palette[c].r;
				gg += project.palette[c].g;
				bb += project.palette[c].b;
				ll++;
			}

			c = canvas->frame->ReadPixel( xx>>canvas->scale, p_y>>canvas->scale );
			rr += (project.palette[c].r*5);
			gg += (project.palette[c].g*5);
			bb += (project.palette[c].b*5);

			p[0] = rr/(ll+5);
			p[1] = gg/(ll+5);
			p[2] = bb/(ll+5);
		
		} else {

			c = canvas->frame->ReadPixel( xx>>canvas->scale, p_y>>canvas->scale );
			
			p[0] = project.palette[c].r;
			p[1] = project.palette[c].g;
			p[2] = project.palette[c].b;
			
		}
		
		p += 3;
		
	}
	
}

void Fl_CanvasClass::draw() {
	
	Fl_Box::draw();
	
	if( frame ) {
		
		fl_draw_image( draw_func, this, x()+1, y()+1, w()-5, h()-5, 3 );
		
	}
	
}

void Fl_CanvasClass::SetScale(int sc) {
	
	scale = sc;

	resize( x(), y(), (frame->w()<<scale)+5, (frame->h()<<scale)+5 );
	
	parent()->redraw();
	
}