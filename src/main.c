#include <gst/gst.h>

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
//nclude <ncurses.h>
#include <sys/ioctl.h>
#include <termios.h>


#include "intercomMonitor.h"

void start (void);

pthread_t videoBaseThreadID;
pthread_t videoFloor1hreadID;

//volatile threadStatus_t videoBaseStatus = { 0, false, false,NULL };
//volatile threadStatus_t videoFloor1Status = { 0, false, false,NULL };

struct thread_info videoBaseInfo, videoFloor1Info;

static void* videoThread(void *arg) {
	struct thread_info *tinfo = arg;

	GstElement *pipeline, *source, *sink;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	/* Create the elements */
	source = gst_element_factory_make("videotestsrc", "source");
	sink = gst_element_factory_make("autovideosink", "sink");

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new("test-pipeline");

	if (!pipeline || !source || !sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Build the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
	if (gst_element_link(source, sink) != TRUE) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Modify the source's properties */
	g_object_set(source, "pattern", 0, NULL);

	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
			GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* Parse message */
	if (msg != NULL) {
		GError *err;
		gchar *debug_info;

		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &err, &debug_info);
			g_printerr("Error received from element %s: %s\n",
					GST_OBJECT_NAME(msg->src), err->message);
			g_printerr("Debugging information: %s\n",
					debug_info ? debug_info : "none");
			g_clear_error(&err);
			g_free(debug_info);
			break;
		case GST_MESSAGE_EOS:
			g_print("End-Of-Stream reached.\n");
			break;
		default:
			/* We should not reach here because we only asked for ERRORs and EOS */
			g_printerr("Unexpected message received.\n");
			break;
		}
		gst_message_unref(msg);
	}

	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}



int main(int argc, char *argv[]) {
	int result;

	  /* Initialize GTK */
	  gtk_init (&argc, &argv);

	  /* Initialize GStreamer */
	  gst_init (&argc, &argv);

	start();
//	initScreen();

//
//	/* Initialize GStreamer */
//	gst_init(&argc, &argv);
//
//	result = pthread_create(&videoBaseInfo.thread_id, NULL, &videoThread,
//			&videoBaseInfo);
//	result = pthread_create(&videoFloor1Info.thread_id, NULL, &videoThread,
//			&videoFloor1Info);
//
//
//	//result = pthread_create(&i2cThreadID, NULL, &videoThread, (void *) &videoBaseStatus);
//	if (result == 0) {
//		printf("videoThread created successfully.\n");
//	} else {
//		printf("videoThread not created.\n");
//	}
//	while  (!kbhit())

	sleep(1);
}



