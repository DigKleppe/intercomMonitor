#include <gst/gst.h>

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "intercomMonitor.h"
#include "ports.h"
#include "video.h"
#include "audio.h"

int screenMain(int argc, char **argv);

int main(int argc, char *argv[]) {

//	GstStateChangeReturn ret;
	/* Initialize GTK */
//	gtk_init(&argc, &argv);

	/* Initialize GStreamer */
	gst_init(&argc, &argv);

	startVideo(VIDEOPORT1, 0);
	startVideo(VIDEOPORT2, 1);

	startAudio(AUDIO_RX_PORT1,0);
	startAudio(AUDIO_RX_PORT2,1);

	screenMain(argc, argv);

	gst_object_unref(videoPipeline[0]);
	gst_object_unref(videoPipeline[1]);
	gst_object_unref(audioPipeline[0]);
	gst_object_unref(audioPipeline[1]);
}

