#ifndef __APP_H
#define __APP_H
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btstack.h"

#define BD_ADDR_LEN		6
#define DEV_CLASS_LEN	3
#define DEV_NAME_MAX	256

enum { AUDIO_INSTANCE_HFP = 0, AUDIO_INSTANCE_A2DP };
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
	/*HFP EVENT*/
	APP_EVT_HFP_CONN		= 0x00,
	APP_EVT_HFP_DISCONN,
	APP_EVT_HFP_INCOMING_CALL,
	APP_EVT_HFP_OUTGOING_CALL,
	APP_EVT_HFP_CALLING_DEV,
	APP_EVT_HFP_CALLING_PHONE,

	/*A2DP EVENT*/
	APP_EVT_A2DP_CONN	= 0x100,
	APP_EVT_A2DP_DISCONN,

	/*AVRCP EVENT*/
	APP_EVT_AVRCP_CONN	= 0x200,
	APP_EVT_AVRCP_DISCONN,
	APP_EVT_AVRCP_STOP,
	APP_EVT_AVRCP_PLAY,
	APP_EVT_AVRCP_PAUSE,

	/*GATT EVENT*/
	APP_EVT_GATT_CONN	= 0x300,
	APP_EVT_GATT_DISCONN,
	APP_EVT_GATT_DATA_RCV,

	/*SPP EVENT*/
	APP_EVT_SPP_CONN	= 0x400,
	APP_EVT_SPP_DISCONN,
	APP_EVT_SPP_DATA_RCV,

	/*HID EVENT*/
	APP_EVT_HID_CONN	= 0x500,
	APP_EVT_HID_DISCONN,
	APP_EVT_HID_REPORT_KEYBOARD,
	APP_EVT_HID_REPORT_MOUSE,
	APP_EVT_HID_REPORT_GAMEPAD,

	/**/
	APP_EVT_MAX = 0x600,
};

typedef struct device_info {
	uint8_t bd_addr_t[BD_ADDR_LEN];
	uint8_t dev_class[DEV_CLASS_LEN];
	bd_addr_type_t dev_addr_type;
	uint8_t rssi;
	uint8_t dev_type;
	char dev_name[DEV_NAME_MAX];
};
//typedef void(*le_spp_data_ind_handler_t)(uint8_t *, uint16_t);
typedef void(*btstack_evt_handler_t)(uint16_t, uint8_t *, uint16_t);
typedef void(*btstack_evt_display_handler_t)(uint16_t, uint8_t *, uint16_t);
typedef void(*btstack_display_scan_result_t)(int, struct device_info *);

typedef int(*audio_in_open_t)(uint8_t instance, uint32_t nSamplesPerSec, uint16_t wBitsPerSample, uint16_t nChannels);
typedef int(*audio_in_read_t)(uint8_t instance, uint8_t read_type, uint8_t *buf, uint16_t len);
typedef int(*audio_in_write_t)(uint8_t instance);
typedef int(*audio_in_close_t)(uint8_t instance);
typedef int(*audio_in_ioctl_t)(uint8_t instance, uint8_t ioctl);
typedef int(*audio_out_open_t)(uint8_t instance, uint32_t nSamplesPerSec, uint16_t wBitsPerSample, uint16_t nChannels);
typedef int(*audio_out_read_t)(uint8_t instance);
typedef int(*audio_out_write_t)(uint8_t instance, uint8_t *buf, uint16_t len);
typedef int(*audio_out_close_t)(uint8_t instance);
typedef int(*audio_out_ioctl_t)(uint8_t instance, uint8_t ioctl);

typedef struct audio_operation_t {
	/*in*/
	audio_in_open_t aui_open;
	audio_in_read_t aui_read;
	audio_in_write_t aui_write;
	audio_in_close_t aui_close;
	audio_in_ioctl_t aui_ioctl;
	/*out*/
	audio_out_open_t auo_open;
	audio_out_read_t auo_read;
	audio_out_write_t auo_write;
	audio_out_close_t auo_close;
	audio_out_ioctl_t auo_ioctl;
}audio_operation;


void app_init(void);
void btstack_evt_display_handler_regeister(btstack_evt_display_handler_t);
void app_audio_operation_regeister(audio_operation *opt);
//void btstack_display_scan_resault_regeister(btstack_display_scan_result_t);
/*******************************peripheral***********************************/
/*------hfp hf------*/
int app_hfp_hf_init(void);
void hfp_hf_cmd_control(char cmd, void *param);
void hfp_hf_evt_handler_register(btstack_evt_handler_t func);
void sco_data_out_int_operation_regeister(audio_operation *opt);
/*------audio sink------*/
int app_audio_sink_init(void);
void audio_sink_cmd_control(char cmd, void *param);
void avrcp_browsing_cmd_control(char cmd, void *param);
void audio_sink_evt_handler_register(btstack_evt_handler_t func);
void audio_sink_out_in_operation_regeister(audio_operation *opt);
/*------le server------*/
int app_le_streamer_init(void);
int le_streamer_send_data(uint8_t * data, uint16_t len);
void le_server_evt_handler_register(btstack_evt_handler_t func);
/*------spp server------*/
int app_spp_streamer_init(void);
int spp_streamer_send_data(uint8_t * data, uint16_t len);
void spp_server_evt_handler_register(btstack_evt_handler_t func);

/******************************central************************************/
/*------hfp ag------*/
int app_gap_inquiry_init(void);
void gap_inquiry_display_result_regeister(btstack_display_scan_result_t func);
int app_gap_inquiry_scan_set(int);
int app_hfp_ag_init(void);
void hfp_ag_cmd_control(char cmd, void *param);
void hfp_ag_evt_handler_register(btstack_evt_handler_t func);
/*------audio source------*/
int app_audio_source_init(void);
void audio_source_cmd_control(char cmd, void *param);
void audio_source_evt_handler_register(btstack_evt_handler_t func);
/*------le client------*/
int app_le_streamer_client_init(void);
void le_streamer_client_scan_set(int);
uint8_t le_client_conn_disconn(int, bd_addr_t, int);
int le_streamer_client_send_data(uint8_t * data, uint16_t len);
void le_client_evt_handler_register(btstack_evt_handler_t func);
void le_client_btstack_display_scan_resault_regeister(btstack_display_scan_result_t);
/*------spp client------*/
int app_spp_streamer_client_init(void);
uint8_t spp_client_conn_disconn(int conn, bd_addr_t addr);
int spp_streamer_client_send_data(uint8_t * data, uint16_t len);
void spp_client_evt_handler_register(btstack_evt_handler_t func);
#endif
