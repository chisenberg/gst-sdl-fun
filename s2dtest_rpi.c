#include <simple2d.h> // Simple2D
#include <stdio.h> // sprintf
#include <gst/gst.h>

// gcc s2dtest_rpi.c -lsimple2d -I/usr/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L/usr/lib -Wl,-rpath=/opt/vc/lib -Wl,--enable-new-dtags -lSDL2 -lm -I/opt/vc/include/ -L/opt/vc/lib -lGLESv2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

// gcc s2dtest_rpi.c `simple2d --libs` `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest



S2D_Window *window;
S2D_Image  *gst_s2d_image;
S2D_Image  *button;

S2D_Text   *txt_msg;
const char *font = "media/UbuntuMono-R.ttf";
//const char *font = "media/Roboto-Condensed.otf";

// para o gstreamer
GstElement *pipeline, *source, *sink, *rate;
GstCaps *caps;

gint width, height;
GstSample *sample;
gchar *descr;
GError *error = NULL;
gint64 duration, position;
GstStateChangeReturn ret;
gboolean res;
GstMapInfo map;

Uint32 lastTime = 0;
gint64 lastFrameCount = 0;

float currentFps = 0;

S2D_Image *S2D_CreateImageGST(int width, int heigth) {
  
	S2D_Image *img = (S2D_Image *) malloc(sizeof(S2D_Image));
	// here is the secret
	img->surface = SDL_CreateRGBSurface(0,width,height,32, 0, 0, 0, 0);

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
	img->orig_height = heigth;
	img->width  = width;
	img->height = heigth;
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


    txt_msg = S2D_CreateText(font, "Testando texto 1234% ! [/chdata]", 14);
	txt_msg->x = 50;
	txt_msg->y = 500;
	// txt_msg->color.r = 0;
	// txt_msg->color.g = 0;
	// txt_msg->color.b = 0;

	gst_s2d_image->x = 0;
    
	// bind para poder setar pixels dessa textura
       glBindTexture(GL_TEXTURE_2D, gst_s2d_image->texture_id);

}

int initGST2() {

    /* Create the elements */
	source = gst_element_factory_make ("videotestsrc", "source");
	sink = gst_element_factory_make ("appsink", "sink");

    g_object_set (sink, "max-buffers", 5, NULL);
    g_object_set (sink, "drop", 1, NULL);


    /* sink Capabilites */
	caps = gst_caps_new_simple (
		"video/x-raw",
		"format", G_TYPE_STRING, "RGB",
		"width", G_TYPE_INT, gst_s2d_image->orig_width,
		"height", G_TYPE_INT, gst_s2d_image->orig_height,
		"pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
		NULL
	);

    /* Create the empty pipeline */
	pipeline = gst_pipeline_new ("test-pipeline");

	if (!pipeline || !source || !sink) {
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}

    /* Build the pipeline */
	gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
	/* Add the filter caps between source and sink */
	if (gst_element_link_filtered (source, sink, caps) != TRUE) {
		g_printerr ("Elements could not be linked.\n");
		gst_object_unref (pipeline);
		return -1;
	}
	
	/* get the preroll buffer from appsink */
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
	
}

int initGST() {

	pipeline = gst_parse_launch ("v4l2src device=/dev/video2 \
	! image/jpeg,width=1280,height=720,framerate=30/1 \
	! videorate name=rate ! image/jpeg,width=1280,height=720,framerate=30/1 ! jpegdec ! queue \
   	! videoconvert ! video/x-raw,width=1280,height=720,format=RGB,pixel-aspect-ratio=1/1 \
    ! appsink max-buffers=5 drop=1 wait-on-eos=false emit-signals=true name=sink", NULL);
    
	//source = gst_bin_get_by_name(GST_BIN(pipeline), "source");
	sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
	rate = gst_bin_get_by_name(GST_BIN(pipeline), "rate");
	
	if (!pipeline || !sink) {
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}
	
	/* get the preroll buffer from appsink */
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
	
}

void updateTexture() {

	g_signal_emit_by_name (sink, "pull-sample", &sample, NULL);

	/* if we have a buffer now, convert it to a pixbuf */
	if (sample) {

		GstBuffer *buffer;
		GstCaps *caps;
		GstStructure *s;

		/* get the caps structure */
		caps = gst_sample_get_caps (sample);
		s = gst_caps_get_structure (caps, 0);
		gst_structure_get_int (s, "width", &width);
		gst_structure_get_int (s, "height", &height);

		/* get the pixbuf */
		buffer = gst_sample_get_buffer (sample);

		if (gst_buffer_map (buffer, &map, GST_MAP_READ)) {
			glBindTexture(GL_TEXTURE_2D, gst_s2d_image->texture_id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1280, 720, GL_RGB, GL_UNSIGNED_BYTE, map.data);
			gst_buffer_unmap (buffer, &map);
		} else {
			printf ("could not map\n");
		}

		gst_sample_unref (sample);

	} else {
		printf ("could not make snapshot\n");
	}
}

// função de desenho, é chamada na mesma frequência que o fps (ex 30fps)
void render () {
     S2D_DrawImage(gst_s2d_image);
     S2D_DrawImage(button);
	
	S2D_DrawQuad(0, 0, 0, 1, 1, 0.5f,
            480, 0, 0, 1, 1, 0.5f,
            480, 320, 0, 1, 1, 0.5f,
            0, 320, 0, 1, 1, 0.5f);
	S2D_DrawText(txt_msg);

	// call once to pull gst frame
	updateTexture();
}

// função de lógica, é chamada mais vezes que a render
void update() {
	// updateTexture();
}


int main ( int argc, char **argv ) {

    gst_init(&argc, &argv);

    // carrega recursos
	load();

    initGST();

	S2D_Diagnostics(true);
	// carrega Simple2D
	window = S2D_CreateWindow("AAAA", 1280, 720, update, render, SDL_WINDOW_BORDERLESS);
	window->viewport.mode = S2D_FIXED;
    window->fps_cap = 60;
	window->vsync = false;
	S2D_ShowCursor(true);
	
	// inicia interface e bloqueia
	S2D_Show(window);
	// fim da aplicação
	S2D_FreeWindow(window);

    return 0;
}
