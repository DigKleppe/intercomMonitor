/*
 * audio.h
 *
 *  Created on: Jun 13, 2023
 *      Author: dig
 */

#ifndef INCLUDES_AUDIO_H_
#define INCLUDES_AUDIO_H_

extern GstElement *audioPipeline[2]; // only works global??
GstStateChangeReturn* startAudio(int UDPport, int idx);

#endif /* INCLUDES_AUDIO_H_ */
