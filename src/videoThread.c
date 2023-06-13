/*
 * video.c
 *
 *  Created on: Jan 27, 2019
 *      Author: dig
 */

#include <gst/gst.h>
//#include "telefoon.h"
#include "videoThread.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

gboolean link_elements_with_filter(GstElement *element1, GstElement *element2,
		GstCaps *caps) {
	gboolean link_ok;
	link_ok = gst_element_link_filtered(element1, element2, caps);
	gst_caps_unref(caps);

	if (!link_ok) {
		g_warning("Filter failed to link element1 and element2!");
	}
	return link_ok;
}

//gst-launch-1.0 v4l2src device=/dev/video0 ! image/jpeg,framerate=30/1,width=800,height=600 ! rtpjpegpay ! udpsink port=5001 host=192.168.2.255
//gst-launch-1.0 videotestsrc pattern=zone-plate kx2=20 ky2=20 kt=1 ! video/x-raw,width=800,height=600 ! jpegenc ! rtpjpegpay ! udpsink port=5001 host=192.168.2.255

//gst-launch-1.0 -v udpsrc port=5001 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert !  videoflip method=counterclockwise ! nxvideosink
//gst-launch-1.0 -v udpsrc port=5001 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert !  autovideosink


int updateTextExceptionCntr;
int setVideoExceptionCntr;
int setVideoTextExceptionCntr;

GstElement *videopipeline = NULL;
GstElement *videoSource, *rtpjpegdepay, *jpegdec, *videoconvert, *videoflip,
		*videosink;
GstElement *textoverlay; // used for testscreen



GstElement* startReceiving(int UDPport, char *pText) {
	bool error = false;
	GstCaps *caps;
	GstStateChangeReturn ret;

//	GstElement *videopipeline = NULL;
//	GstElement *videoSource, *rtpjpegdepay, *jpegdec, *videoconvert, *videoflip,
//			*videosink;
//	GstElement *textoverlay; // used for testscreen

//	try {

	//			stopVideo();
	videopipeline = gst_pipeline_new("videopipeline");
	videoSource = gst_element_factory_make("udpsrc", "udpsrc");
	g_object_set(videoSource, "port", UDPport, NULL);

//	sprintf("video stream started port %d\n", UDPport);

	rtpjpegdepay = gst_element_factory_make("rtpjpegdepay", "rtpjpegdepay");
	jpegdec = gst_element_factory_make("jpegdec", "jpegdec");
	videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
	videoflip = gst_element_factory_make("videoflip", "videoflip");
	g_object_set(videoflip, "method", 0, NULL);

	videosink = gst_element_factory_make("autovideosink", "autovideosink");

	if (!videopipeline || !videoSource || !rtpjpegdepay || !jpegdec
			|| !videoconvert || !videoflip || !videosink) {
		g_printerr("Not all elements could be created.\n");
		error = true;
	}
	if (!error) {
		/* Build the pipeline */
		gst_bin_add_many(GST_BIN(videopipeline), videoSource, rtpjpegdepay,
				jpegdec, videoconvert, videoflip, videosink, NULL);

		caps = gst_caps_new_simple("application/x-rtp",
				//	"framerate",GST_TYPE_FRACTION, 30, 1,
				"encoding-name", G_TYPE_STRING, "JPEG", "payload", G_TYPE_INT,
				26,
				NULL);
		if (link_elements_with_filter(videoSource, rtpjpegdepay, caps) != TRUE) {
			g_printerr("rtpjpegdepay elements could not be linked.\n");
			error = true;
		}

		if (gst_element_link(rtpjpegdepay, jpegdec) != TRUE) {
			g_printerr("jpegenc and videopayloader could not be linked.\n");
			error = true;
		}

		if (gst_element_link(jpegdec, videoconvert) != TRUE) {
			g_printerr("jpegenc and videopayloader could not be linked.\n");
			error = true;
		}

		if (gst_element_link(videoconvert, videoflip) != TRUE) {
			g_printerr("jpegenc and videopayloader could not be linked.\n");
			error = true;
		}

		if (gst_element_link(videoflip, videosink) != TRUE) {
			g_printerr("jpegenc and videopayloader could not be linked.\n");
			error = true;
		}
	}
	return videopipeline;
}


