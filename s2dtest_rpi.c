#include <simple2d.h> // Simple2D
#include <stdio.h> // sprintf
#include "src/video_cap.hpp"

// gcc s2dtest_rpi.c -lsimple2d -I/usr/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L/usr/lib -Wl,-rpath=/opt/vc/lib -Wl,--enable-new-dtags -lSDL2 -lm -I/opt/vc/include/ -L/opt/vc/lib -lGLESv2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

// gcc s2dtest_rpi.c `simple2d --libs` `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

S2D_Window *window;
S2D_Image  *gst_s2d_image;
S2D_Image  *button;

S2D_Text   *txt_msg;
const char *font = "media/UbuntuMono-R.ttf";
// const char *font = "media/telegren.ttf";

VideoCap *video_cap;

S2D_Image *S2D_CreateImageGST(int width, int height) {
  
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


// carrega os recursos
void load () {
	button = S2D_CreateImage("media/button.png");
	gst_s2d_image = S2D_CreateImageGST(1280,720);

	txt_msg = S2D_CreateText(font, "Testando texto 1234% ! [/chdata]", 16);
	txt_msg->x = 50;
	txt_msg->y = 500;
	// txt_msg->color.r = 0;
	// txt_msg->color.g = 0;
	// txt_msg->color.b = 0;

	gst_s2d_image->width = 1100;
}

// Simple2D render function
void render () {

	video_cap->updateGLTexture(gst_s2d_image->texture_id);
	S2D_DrawImage(gst_s2d_image);
	
	S2D_DrawQuad(0, 0, 0, 1, 1, 0.5f,
		480, 0, 0, 1, 1, 0.5f,
		480, 320, 0, 1, 1, 0.5f,
		0, 320, 0, 1, 1, 0.5f);
		
	S2D_DrawImage(button);
	S2D_DrawText(txt_msg);

}

// Simple2D logics function
void update() {
}


int main ( int argc, char **argv ) {

	gst_init(&argc, &argv);

	load();
	video_cap = new VideoCap(1280,720);

	S2D_Diagnostics(true);
	// carrega Simple2D
	window = S2D_CreateWindow("AAAA", 1366, 720, update, render, SDL_WINDOW_BORDERLESS);
	window->viewport.mode = S2D_FIXED;
    window->fps_cap = 60;
	window->vsync = false;
	// S2D_ShowCursor(true);
	
	// inicia interface e bloqueia
	S2D_Show(window);
	// fim da aplicação
	S2D_FreeWindow(window);

    return 0;
}
