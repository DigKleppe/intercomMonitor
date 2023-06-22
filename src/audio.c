/*
 * audio.c
 *
 *  Created on: Jun 13, 2023
 *      Author: dig
 *
 *
 *
 gst-launch-1.0 udpsrc port=5002 caps='application/x-rtp, media=(string)audio, clock-rate=(int)44100, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, payload=(int)96' ! rtpL16depay ! audioconvert ! autoaudiosink

 gst-launch-1.0 udpsrc port=5002 caps='application/x-rtp, media=(string)audio, clock-rate=(int)44100, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, payload=(int)96' ! rtpL16depay ! tee name= t t. ! queue  ! audioconvert ! autoaudiosink t.! queue ! audioconvert ! wavescope ! ximagesink
 *
 */

#include <stdbool.h>
#include <gst/gst.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "video.h"

#define SPEAKERCHANNEL			1.0   // panorama
#define HANDSETCHANNEL			-1.0
#define BOTHCHANNELS			0
#define HANDSETCHANNEL			-1.0
#define TESTVOLUME				0.3
#define HANDSETVOLUME 1.0

GstElement *audioPipeline[2]; // only works global??

GstStateChangeReturn startAudio(int UDPport, int idx) {

	GstElement *audioSource, *rtpL16depay, *audioconvert, *audioconvert2, *volume, *audiopanorama, *audioresample, *audiosink, *tee;
	GstElement *queue, *queue2, *wavescope, *ximagesink;

	bool error = false;
	GstCaps *caps = NULL;
	GstStateChangeReturn ret;
	printf("audio listen started port %d\n", UDPport);

	audioSource = gst_element_factory_make("udpsrc", "udpsrc");
	g_object_set(audioSource, "port", UDPport, NULL);

	rtpL16depay = gst_element_factory_make("rtpL16depay", "rtpL16depay");

	audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
	audioresample = gst_element_factory_make("audioresample", "audioresample");
	audiopanorama = gst_element_factory_make("audiopanorama", "audiopanorama");
	g_object_set(audiopanorama, "panorama", HANDSETCHANNEL, NULL); // sound to handset

	volume = gst_element_factory_make("volume", "volume");
	g_object_set(volume, "volume", HANDSETVOLUME, NULL);

	audiosink = gst_element_factory_make("autoaudiosink", "autoaudiosink");
	gst_element_factory_make("audioconvert", "audioconvert");

	tee = gst_element_factory_make("tee", "tee");
	g_object_set(tee, "name", "t", NULL);

	audioconvert2 = gst_element_factory_make("audioconvert", "audioconvert2");
	queue = gst_element_factory_make("queue", "queue");
	queue2 = gst_element_factory_make("queue", "queue2");
	wavescope = gst_element_factory_make("wavescope", "wavescope");
	ximagesink = gst_element_factory_make("ximagesink", "ximagesink");

//	audiosink = gst_element_factory_make("alsasink", "alsasink");
//	g_object_set(audiosink, "device", devicename, NULL);

	if (!audioSource || !rtpL16depay || !audioconvert || !audioresample || !audiopanorama || !audiosink || !volume || !tee || !queue || !queue2
			|| !audioconvert2 || !wavescope || !ximagesink) {
		g_printerr("Not all audio elements could be created.\n");
		error = true;
	}

	audioPipeline[idx] = gst_pipeline_new("receive audiopipeline");
	gst_bin_add_many(GST_BIN(audioPipeline[idx]), audioSource, rtpL16depay, audioconvert, audioconvert2, audioresample, volume, audiopanorama, audiosink, tee,
			queue, queue2, wavescope, ximagesink,
			NULL);

	caps = gst_caps_new_simple("application/x-rtp", "media", G_TYPE_STRING, "audio",
	//	"clock-rate", G_TYPE_INT,24000, // logitec
			"clock-rate", G_TYPE_INT, 44100, // USB sound
			"encoding-name", G_TYPE_STRING, "L16", "encoding-params", (G_TYPE_STRING), "1", "channels", G_TYPE_INT, 1, "payload",
			G_TYPE_INT, 96, NULL);

	if (link_elements_with_filter(audioSource, rtpL16depay, caps) != TRUE)
		error = true;

	if (gst_element_link(rtpL16depay, tee) != TRUE)
		error = true;

	if (gst_element_link(tee, queue) != TRUE)
		error = true;

	if (gst_element_link(queue, audioconvert) != TRUE)
		error = true;

//	if (gst_element_link(rtpL16depay, audioconvert) != TRUE)
//		error = true;

	if (gst_element_link(audioconvert, audioresample) != TRUE)
		error = true;

	if (gst_element_link(audioresample, volume) != TRUE)
		error = true;

	if (gst_element_link(volume, audiopanorama) != TRUE)
		error = true;

	if (gst_element_link(audiopanorama, audiosink) != TRUE)
		error = true;

	if (gst_element_link(tee, queue2) != TRUE)
		error = true;

	if (gst_element_link(queue2, audioconvert2) != TRUE)
		error = true;

	if (gst_element_link(audioconvert2, wavescope) != TRUE)
		error = true;

	if (gst_element_link(wavescope, ximagesink) != TRUE)
		error = true;

	usleep(10 * 1000);
	if (error)
		g_printerr("Elements could not be linked.\n");
	else {
		ret = gst_element_set_state(audioPipeline[idx], GST_STATE_PLAYING);
		if (ret == GST_STATE_CHANGE_FAILURE) {
			g_printerr(" audio Receive Unable to set the audio pipeline to the playing state.\n");
			error = true;
		}
	}

	return ret;
}
