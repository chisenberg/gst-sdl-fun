#include <simple2d.h> // Simple2D
#include <stdio.h> // sprintf
#include <gst/gst.h>

// gcc s2dtest_rpi.c -lsimple2d -I/usr/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L/usr/lib -Wl,-rpath=/opt/vc/lib -Wl,--enable-new-dtags -lSDL2 -lm -I/opt/vc/include/ -L/opt/vc/lib -lGLESv2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest

// gcc s2dtest_rpi.c `simple2d --libs` `pkg-config --cflags --libs gstreamer-1.0` -o s2dtest



S2D_Window *window;
S2D_Image  *img_1280;
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


float getCpuUsage () {
	
	long double a[4], b[4], loadavg;
    FILE *fp;

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
}

// carrega os recursos
void load () {
	
	img_1280 = S2D_CreateImage("media/1280.png");

    txt_msg = S2D_CreateText(font, "Testando texto 1234% ! [/chdata]", 14);
	txt_msg->x = 50;
	txt_msg->y = 500;
	// txt_msg->color.r = 0;
	// txt_msg->color.g = 0;
	// txt_msg->color.b = 0;

    img_1280->width = 480;
    img_1280->height = 320;
    
	// bind para poder setar pixels dessa textura
       glBindTexture(GL_TEXTURE_2D, img_1280->texture_id);

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
		"width", G_TYPE_INT, img_1280->orig_width,
		"height", G_TYPE_INT, img_1280->orig_height,
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
	! image/jpeg,width=640,height=480,framerate=15/1 \
	! videorate name=rate ! image/jpeg,width=640,height=480,framerate=15/1 ! jpegdec ! queue \
   	! videoscale ! queue ! videoconvert ! video/x-raw,width=480,height=320,format=RGB,pixel-aspect-ratio=1/1 \
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

	    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 480, 320, GL_RGB, GL_UNSIGNED_BYTE, map.data);
            
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
	updateTexture();
	
	// S2D_DrawQuad(0, 0, 0, 1, 1, 1,
       //      480, 0, 0, 1, 1, 1,
          //   480, 320, 0, 1, 1, 1,
            // 0, 320, 0, 1, 1, 1);
     S2D_DrawImage(img_1280);
	// S2D_DrawText(txt_msg);
	
//	S2D_DrawImage(screen);

	// printf("\nFPS: %f", calcFps());
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
	window = S2D_CreateWindow("AAAA", 480, 320, update, render, S2D_RESIZABLE);
	window->viewport.mode = S2D_FIXED;
    window->fps_cap = 30;
	window->vsync = false;
	S2D_ShowCursor(true);
	
	// inicia interface e bloqueia
	S2D_Show(window);
	// fim da aplicação
	S2D_FreeWindow(window);

    return 0;
}
