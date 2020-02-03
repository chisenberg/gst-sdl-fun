#ifndef S2D_INFO
#include <simple2d.h>
#endif
#include <SDL2/SDL.h>

S2D_Image *S2D_CreateEmptyImage(int width, int height) {
  
	S2D_Image *img = (S2D_Image *) malloc(sizeof(S2D_Image));
	// here is the secret
	img->surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

	// Default values
	img->texture_id = 0;
	img->path = "";
	img->x = 0;
	img->y = 0;
	img->color.r = 1.f;
	img->color.g = 1.f;
	img->color.b = 1.f;
	img->color.a = 1.f;
	img->orig_width  = width;
	img->orig_height = height;
	img->width  = width;
	img->height = height;
	img->rotate = 0;
	img->rx = 0;
	img->ry = 0;
	img->format = GL_RGB;

	return img;
}

void S2D_DrawTextShadow(S2D_Text *txt) {
	if (!txt) return;

	if (txt->texture_id == 0) {
		int outline_size = 2;
		SDL_Color color = { 255, 255, 255 };
		SDL_Color shadow_color = { 0, 0, 0 };
		SDL_Surface *fg_surface = TTF_RenderText_Blended(txt->font_data, txt->msg, color);

		/* update same font with outline and recalculate total width and height */
		TTF_SetFontOutline(txt->font_data, outline_size); 
		TTF_SizeText(txt->font_data, txt->msg, &txt->width, &txt->height);
		SDL_Surface *bg_surface = TTF_RenderText_Blended(txt->font_data, txt->msg, shadow_color);
		SDL_Rect rect = {outline_size, outline_size, fg_surface->w, fg_surface->h}; 

		/* blit foreground into background */
		SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
		SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
		
		if (!bg_surface) {
			S2D_Error("TTF_RenderText_Blended", TTF_GetError());
			return;
		}
		S2D_GL_CreateTexture(&txt->texture_id, GL_RGBA,
			txt->width, txt->height,
			bg_surface->pixels, GL_NEAREST
		);
		SDL_FreeSurface(fg_surface);
		SDL_FreeSurface(bg_surface);
	}

	S2D_GL_DrawText(txt);
}