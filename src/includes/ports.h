/*
 * ports.h
 *
 *  Created on: Jun 9, 2023
 *      Author: dig
 */

#ifndef INCLUDES_PORTS_H_
#define INCLUDES_PORTS_H_


#define VIDEOPORT1				5000	// camera1 to display (begane grond)
#define VIDEOPORT2				5001	// camera2 to display (eerste etage)
#define AUDIO_RX_PORT1			5002	// mic camera 1 to stations
#define AUDIO_RX_PORT2			5003	// mic camera 2 to stations
#define AUDIO_TX_PORT1			5004	// stations to door speaker camera 1
#define AUDIO_TX_PORT2			5005	// stations to door speaker camera 2
#define RECEIVE_STATIONSPORT1	5010	// incoming calls from stations to camera 1
#define RECEIVE_STATIONSPORT2	5011	// incoming calls from stations to camera 2
#define TCPCHATPORT				5012	// tcp socket for chatting between cameras and telephones
#define COMMANDPORT				5013	// tcp socket for commands to telephones
#define UDPCHATPORT				5014	// udp socket for chatting between cameras and telephones
#define UDPCHATPORT2			5015	// udp socket for chatting between cameras and telephones
#define UDPKEEPALIVEPORT		5016
#define UDPACKPORT1				5017	// acks from camera 1
#define UDPACKPORT2				5018	// acks from camera 2

#define UPDATEPORT				5050	// tcp socket for update files to telephones

#define MONITORTELEPHONESPORT	6003 // 6000 6001 used

#endif /* INCLUDES_PORTS_H_ */
