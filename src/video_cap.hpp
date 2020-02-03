#include <gst/gst.h>
#include <GL/gl.h>
#include <stdio.h>

#ifndef __GST__H__
#define __GST__H__

class VideoCap
{
private:
	GstElement* sink;
	int width;
	int height;

	static int new_sample;
	static GstFlowReturn newSampleSignal (GstElement *sink, void *data);

public:
	void updateGLTexture(GLuint texture_id);
	VideoCap(int width, int heigth);
};

#endif