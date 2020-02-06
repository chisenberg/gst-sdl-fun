#ifndef S2D_INFO
#include <simple2d.h>
#endif

#include "sd2_util.hpp"

void drawStep(int x, int y, int w, int h, SDL_Color color, int h_flip)
{
	// shadow
	int border = 2;
	int x_border = h_flip ? -border : border;
	w = h_flip ? -w : w;
	S2D_DrawQuad(
		x-x_border, y-border, 0, 0, 0, 0.8f,
		x+w+x_border, y-border, 0, 0, 0, 0.8f,
		x+w+x_border, y+h+border, 0, 0, 0, 0.8f,
		x-x_border, y+h+border, 0, 0, 0, 0.8f
	);
	S2D_DrawQuad(
		x, y, color.r, color.g, color.b, 1,
		x+w, y, color.r, color.g, color.b, 1,
		x+w, y+h, color.r, color.g, color.b, 1,
		x, y+h,	color.r, color.g, color.b, 1
	);
}
	
void drawVerticalGauge(int value, int step, int x_center, int y_center, S2D_Text *text, S2D_Image *tick, SDL_Color color, int h_flip)
{
	int width = 8;
	int height = 2;
	int space = 8;
	
	int is_double = 0;

	int x_dir = h_flip ? -1 : 1; 

	// drawStep(x_center + (width*5)*x_dir, y_center, width*2, height, color, h_flip);

	/**
	 * 
	 * Criar array de S2D_Text para 8 ou 10
	 * Apenas atualize o texto se precisar
	 * Desenhar apenas os que são utilizados
	 * 
	 */

	int iterator = value + 20;
	int y_pos = 0;
	text->color.r = color.r;
	text->color.g = color.g;
	text->color.b = color.b;
	tick->x = x_center;

	for (int i=-20; i<20; i++) {
		// if is a multiple of step
		is_double = iterator%step == 0 ? 1 : 0;
		// calculate Y step position
		y_pos = y_center + ((height + space) * i);
		// text props
		S2D_SetText(text, "%d", iterator);
		text->y = y_pos - (text->height/2);
		text->x = x_center + ((width * 2) + 5) * x_dir - (h_flip?text->width:0);

		if(is_double) {
			S2D_DrawTextShadow(text);
			// S2D_DrawText(text);
			drawStep(x_center, y_pos, width*2, height, color, h_flip);
		} else {
			drawStep(x_center, y_pos, width, height, color, h_flip);
		}
		// tick->y = y_pos;
		// S2D_DrawImage(tick);
		iterator--;
	}

}