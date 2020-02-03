#ifndef S2D_INFO
#include <simple2d.h>
#endif

#include "sd2_util.hpp"

void drawStep(int x, int y, int w, int h, SDL_Color color)
{
	// shadow
	int border = 2;
	S2D_DrawQuad(
		x-border, y-border, 0, 0, 0, 0.8f,
		x+w+border, y-border, 0, 0, 0, 0.8f,
		x+w+border, y+h+border, 0, 0, 0, 0.8f,
		x-border, y+h+border,	0, 0, 0, 0.8f
	);
	S2D_DrawQuad(
		x, y, color.r, color.g, color.b, 1,
		x+w, y, color.r, color.g, color.b, 1,
		x+w, y+h, color.r, color.g, color.b, 1,
		x, y+h,	color.r, color.g, color.b, 1
	);
}
	

void drawVerticalGauge(int value, int step, int x_center, int y_center, S2D_Text *text, SDL_Color color)
{
	int width = 12;
	int height = 3;
	int space = 8;
	
	int is_double = 0;

	drawStep(x_center - width*7, y_center, width*2, 3, color);

	int iterator = value + 20;
	int y_pos = 0;
	text->color.r = color.r;
	text->color.g = color.g;
	text->color.b = color.b;

	for (int i=-20; i<20; i++) {
		// if is a multiple of step
		is_double = iterator%step == 0 ? 1 : 0;
		// calculate Y step position
		y_pos = y_center + ((height + space) * i);
		// text props
		S2D_SetText(text, "%d", iterator);
		text->y = y_pos - (text->height/2);
		text->x = x_center - text->width - width - 5;

		if(is_double) {
			// S2D_DrawTextShadow(text);
			S2D_DrawText(text);
			drawStep(x_center - width, y_pos, width*2, height, color);
		} else {
			drawStep(x_center, y_pos, width, height, color);
		}
		iterator--;
	}

}