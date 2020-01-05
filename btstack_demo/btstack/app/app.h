#ifndef __APP_H
#define __APP_H
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BD_ADDR_LEN		6
#define DEV_CLASS_LEN	3
#define DEV_NAME_MAX	256

enum _stack_event_type {
	STACK_EVENT_TYPE_HFP,
	STACK_EVENT_TYPE_A2DP,
	STACK_EVENT_TYPE_AVRCP,
	STACK_EVENT_TYPE_GATT,
	STACK_EVENT_TYPE_SPP,
	STACK_EVENT_TYPE_HID,
	STACK_EVENT_TYPE_INVAILD,
};

enum _stack_event {
	/*HFP HF EVENT*/
	APP_EVT_HFP_HF_CONN,
	APP_EVT_HFP_HF_DISCONN,
	APP_EVT_HFP_HF_INCOMING_CALL,
	APP_EVT_HFP_HF_OUTGOING_CALL,
	APP_EVT_HFP_HF_CALLING,
	APP_EVT_HFP_HF_CALLING_PHONE,

	/*A2DP SINK EVENT*/
	APP_EVT_A2DP_SINK_CONN,
	APP_EVT_A2DP_SINK_DISCONN,

	/*AVRCP CT EVENT*/
	APP_EVT_AVRCP_CT_CONN,
	APP_EVT_AVRCP_CT_DISCONN,
	APP_EVT_AVRCP_CT_STOP,
	APP_EVT_AVRCP_CT_PLAY,
	APP_EVT_AVRCP_CT_PAUSE,

	/*GATT SERVER EVENT*/
	APP_EVT_GATT_SER_CONN,
	APP_EVT_GATT_SER_DISCONN,
	APP_EVT_GATT_SER_DATA_RCV,

	/*SPP SERVER EVENT*/
	APP_EVT_SPP_SER_CONN,
	APP_EVT_SPP_SER_DISCONN,
	APP_EVT_SPP_SER_DATA_RCV,

	/*HID SERVER EVENT*/
	APP_EVT_HID_SER_CONN,
	APP_EVT_HID_SER_DISCONN,
	APP_EVT_HID_SER_REPORT_KEYBOARD,
	APP_EVT_HID_SER_REPORT_MOUSE,
	APP_EVT_HID_SER_REPORT_GAMEPAD
};

typedef struct device_info {
	unsigned char bd_addr_t[BD_ADDR_LEN];
	unsigned char dev_class[DEV_CLASS_LEN];
	char dev_name[DEV_NAME_MAX];
};
//typedef void(*le_spp_data_ind_handler_t)(uint8_t *, uint16_t);
typedef void(*btstack_evt_handler_t)(uint16_t, uint8_t *, uint16_t);
typedef void(*btstack_evt_display_handler_t)(uint16_t, uint8_t *, uint16_t);

void app_init(void);
void btstack_evt_display_handler_regeister(btstack_evt_display_handler_t);

/*------le server------*/
int app_le_streamer_init(void);
int le_streamer_send_data(uint8_t * data, uint16_t len);
void le_server_evt_handler_register(btstack_evt_handler_t func);
/*------le client------*/
int app_le_streamer_client_init(void);
/*------spp server------*/
int app_spp_streamer_init(void);
int spp_streamer_send_data(uint8_t * data, uint16_t len);
void spp_server_evt_handler_register(btstack_evt_handler_t func);
/*------hfp hf------*/
int app_hfp_hf_init(void);
void hfp_hf_cmd_control(char cmd, void *param);
void hfp_hf_evt_handler_register(btstack_evt_handler_t func);
/*------audio sink------*/
int app_audio_sink_init(void);
void audio_sink_cmd_control(char cmd, void *param);
void avrcp_browsing_cmd_control(char cmd, void *param);
void audio_sink_evt_handler_register(btstack_evt_handler_t func);
#endif
