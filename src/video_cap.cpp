#include "video_cap.hpp"

int VideoCap::new_sample = 0;

GstFlowReturn VideoCap::newSampleSignal (GstElement *sink, void *data)
{
	VideoCap::new_sample = 1;
	return GST_FLOW_OK;
}

VideoCap::VideoCap(int width, int height)
{
	this->width = width;
	this->height = height;

	GstElement *pipeline = gst_parse_launch ("v4l2src device=/dev/video0 \
	! image/jpeg,width=640,height=480,framerate=30/1 ! jpegdec \
	! videoconvert ! video/x-raw,width=640,height=480,format=RGB,pixel-aspect-ratio=1/1 \
	! appsink max-buffers=1 drop=1 wait-on-eos=false sync=false emit-signals=true name=sink", NULL);

	this->sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
	// GstElement *rate = gst_bin_get_by_name(GST_BIN(pipeline), "rate");
	
	if (!pipeline || !sink) {
		g_printerr ("Not all elements could be created.\n");
	}

	// configure callback
	g_signal_connect (this->sink, "new-sample", G_CALLBACK (VideoCap::newSampleSignal), 0);
	
	/* get the preroll buffer from appsink */
	gst_element_set_state (pipeline, GST_STATE_PAUSED);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

}

void VideoCap::updateGLTexture(GLuint texture_id)
{
	if(!this->new_sample) {
		return;
	}
	this->new_sample = 0;

	GstSample *sample;
	g_signal_emit_by_name (this->sink, "pull-sample", &sample, NULL);

	if (sample)
	{
		GstBuffer *buffer;
		GstMapInfo map;

		buffer = gst_sample_get_buffer (sample);

		if (gst_buffer_map (buffer, &map, GST_MAP_READ)) {
			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, map.data);
			gst_buffer_unmap (buffer, &map);
		} else {
			printf ("could not map\n");
		}

		gst_sample_unref (sample);
	} else {
		printf("error getting sample");
	}
}


// int initGST2() {

    // /* Create the elements */
	// source = gst_element_factory_make ("videotestsrc", "source");
	// sink = gst_element_factory_make ("appsink", "sink");

    // g_object_set (sink, "max-buffers", 5, NULL);
    // g_object_set (sink, "drop", 1, NULL);


    // /* sink Capabilites */
	// caps = gst_caps_new_simple (
	// 	"video/x-raw",
	// 	"format", G_TYPE_STRING, "RGB",
	// 	"width", G_TYPE_INT, gst_s2d_image->orig_width,
	// 	"height", G_TYPE_INT, gst_s2d_image->orig_height,
	// 	"pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
	// 	NULL
	// );

    // /* Create the empty pipeline */
	// pipeline = gst_pipeline_new ("test-pipeline");

	// if (!pipeline || !source || !sink) {
	// 	g_printerr ("Not all elements could be created.\n");
	// 	return -1;
	// }

    // /* Build the pipeline */
	// gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
	// /* Add the filter caps between source and sink */
	// if (gst_element_link_filtered (source, sink, caps) != TRUE) {
	// 	g_printerr ("Elements could not be linked.\n");
	// 	gst_object_unref (pipeline);
	// 	return -1;
	// }

	// // configure callback
	// g_signal_connect (sink, "new-sample", G_CALLBACK (new_sample_signal), 0);
	
	// /* get the preroll buffer from appsink */
    // gst_element_set_state (pipeline, GST_STATE_PAUSED);
    // gst_element_set_state (pipeline, GST_STATE_PLAYING);
	
// }