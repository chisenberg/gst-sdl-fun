#include <simple2d.h> // Simple2D
#include <stdio.h> // sprintf
#include "src/sd2_util.hpp"
#include "src/video_cap.hpp"
#include "src/tests.hpp"

// gcc s2dtest_rpi.c -lsimple2d -I/usr/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L/usr/lib -Wl,-rpath=/opt/vc/lib -Wl,--enable-new-dtags -lSDL2 -lm -I/opt/vc/include/ -L/opt/vc/lib -lGLESv2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

// gcc s2dtest_rpi.c `simple2d --libs` `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

S2D_Window *window;
S2D_Image  *gst_s2d_image;
S2D_Image  *button;

S2D_Text   *txt_msg;
S2D_Text   *gauge_text;

const char *font = "media/UbuntuMono-R.ttf";
// const char *font = "media/telegren.ttf";

VideoCap *video_cap;

float gauge_counter = 0;

// carrega os recursos
void load () {
	button = S2D_CreateImage("media/button.png");
	gst_s2d_image = S2D_CreateEmptyImage(1280,720);

	txt_msg = S2D_CreateText(font, "Testando texto 1234% ! [/chdata]", 20);
	
	gauge_text = S2D_CreateText(font, "", 24);
	txt_msg->x = 50;
	txt_msg->y = 50;
	txt_msg->color.r = 0;
	txt_msg->color.g = 1;
	txt_msg->color.b = 1;

	gst_s2d_image->width = 1280;
}



// Simple2D render function
void render () {

	video_cap->updateGLTexture(gst_s2d_image->texture_id);
	S2D_DrawImage(gst_s2d_image);
	
	GLfloat color = 0.2f;
	S2D_DrawQuad(
		1280, 0, 	color, color, color, 1,
		1366, 0, 	color, color, color, 1,
		1366, 720,	color, color, color, 1,
		1280, 720,	color, color, color, 1
	);
		
	// S2D_DrawImage(button);
	S2D_DrawTextShadow(txt_msg);
	

	drawVerticalGauge(gauge_counter,5,200,360, gauge_text, SDL_Color{1,1,1});

	gauge_counter += 0.05;

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
