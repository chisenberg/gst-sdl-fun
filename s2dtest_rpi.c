#include <simple2d.h> // Simple2D
#include <stdio.h> // sprintf
#include "src/sd2_util.hpp"
#include "src/video_cap.hpp"
#include "src/tests.hpp"

// g++ s2dtest_rpi.c src/video_cap.cpp -I/usr/include/SDL2 -D_REENTRANT -Wl,-rpath=/opt/vc/lib -lGLESv2 -lsimple2d -L/usr/lib -pthread -lSDL2 -lGL -lm -lSDL2_image -lSDL2_mixer -lSDL2_ttf `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

// g++ s2dtest_rpi.c src/video_cap.cpp `simple2d --libs` `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

S2D_Window *window;
S2D_Image  *gst_s2d_image;
S2D_Image  *button;
S2D_Image  *tick;

S2D_Text   *txt_msg;
S2D_Text   *gauge_text;

const char *font = "media/UbuntuMono-R.ttf";
// const char *font = "media/telegren.ttf";

VideoCap *video_cap;

float gauge_counter = 0;

// carrega os recursos
void load () {
	button = S2D_CreateImage("media/button.png");
	tick = S2D_CreateImage("media/tick.png");
	gst_s2d_image = S2D_CreateEmptyImage(640,480);

	txt_msg = S2D_CreateText(font, "Testando texto 1234! [/chdata]", 20);
	// S2D_DrawTextShadow(txt_msg);
	//  S2D_SetText(txt_msg, "abacate");

	
	gauge_text = S2D_CreateText(font, "11", 24);
	
	txt_msg->x = 50;
	txt_msg->y = 50;
	txt_msg->color.r = 0;
	txt_msg->color.g = 1;
	txt_msg->color.b = 1;

	gst_s2d_image->width = 640;
	gst_s2d_image->height = 540;
}



// Simple2D render function
void render () {
	
	gst_s2d_image->x = 0;
	gst_s2d_image->y = 0;
	S2D_DrawImage(gst_s2d_image);
	gst_s2d_image->x = 640;
	S2D_DrawImage(gst_s2d_image);
	gst_s2d_image->x = 1280;
	S2D_DrawImage(gst_s2d_image);
	gst_s2d_image->x = 0;
	gst_s2d_image->y = 540;
	S2D_DrawImage(gst_s2d_image);
	gst_s2d_image->x = 640;
	S2D_DrawImage(gst_s2d_image);
	gst_s2d_image->x = 1280;
	S2D_DrawImage(gst_s2d_image);

	GLfloat color = 0.2f;
	// S2D_DrawQuad(
	// 	1280, 0, 	color, color, color, 1,
	// 	1366, 0, 	color, color, color, 1,
	// 	1366, 720,	color, color, color, 1,
	// 	1280, 720,	color, color, color, 1
	// );
		
	// S2D_DrawImage(button);
	

	drawVerticalGauge(gauge_counter,5,200,360, gauge_text, tick, SDL_Color{1,1,1}, true);
	drawVerticalGauge(gauge_counter,10,1080,360, gauge_text, tick, SDL_Color{1,1,1}, false);
	S2D_DrawTextShadow(txt_msg);

	gauge_counter += 0.05;

	video_cap->updateGLTexture(gst_s2d_image->texture_id);

}

// Simple2D logics function
void update() {
}


int main ( int argc, char **argv ) {

	gst_init(&argc, &argv);

	load();
	video_cap = new VideoCap(640,480);

	S2D_Diagnostics(true);
    window = S2D_CreateWindow("", 1280, 720, update, render, S2D_BORDERLESS);
    window->viewport.mode = S2D_FIXED;
    window->fps_cap = 30;
	window->vsync = false;
	S2D_ShowCursor();
	
	// inicia interface e bloqueia
	S2D_Show(window);
	// fim da aplicação
	S2D_FreeWindow(window);

    return 0;
}
