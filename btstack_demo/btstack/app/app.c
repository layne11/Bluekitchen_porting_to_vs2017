#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btstack.h"
#include "app.h"

btstack_evt_display_handler_t btstack_evt_display_handler = NULL;

void btstack_evt_handler(uint16_t evt, void *param, uint16_t len)
{
	if (NULL != btstack_evt_display_handler) {
		(*btstack_evt_display_handler)(evt, param, len);
	}

	switch (evt) {
	 	/*HFP HF EVENT*/
	case APP_EVT_HFP_HF_CONN:
		break;
	case APP_EVT_HFP_HF_DISCONN:
		break;
	case APP_EVT_HFP_HF_INCOMING_CALL:
		break;
	case APP_EVT_HFP_HF_OUTGOING_CALL:
		break;
	case APP_EVT_HFP_HF_CALLING:
		break;
	case APP_EVT_HFP_HF_CALLING_PHONE:
		break;
		/*A2DP SINK EVENT*/
	case APP_EVT_A2DP_SINK_CONN:
		break;
	case APP_EVT_A2DP_SINK_DISCONN:
		break;
		/*AVRCP CT EVENT*/
	case APP_EVT_AVRCP_CT_CONN:
		break;
	case APP_EVT_AVRCP_CT_DISCONN:
		break;
		/*GATT SERVER EVENT*/
	case APP_EVT_GATT_SER_CONN:
		break;
	case APP_EVT_GATT_SER_DISCONN:
		break;
	case APP_EVT_GATT_SER_DATA_RCV:
		break;
		/*SPP SERVER EVENT*/
	case APP_EVT_SPP_SER_CONN:
		break;
	case APP_EVT_SPP_SER_DISCONN:
		break;
	case APP_EVT_SPP_SER_DATA_RCV:
		break;
	}
}
void btstack_evt_display_handler_regeister(btstack_evt_display_handler_t func)
{
	btstack_evt_display_handler = func;
}

void app_init(void)
{
	l2cap_init();
	rfcomm_init();
	sdp_init();
	gap_set_class_of_device(0x200408);
	gap_set_local_name("Bluetooth_test");
	gap_discoverable_control(1);

	hfp_hf_evt_handler_register(btstack_evt_handler);
	audio_sink_evt_handler_register(btstack_evt_handler);
	le_server_evt_handler_register(btstack_evt_handler);
	spp_server_evt_handler_register(btstack_evt_handler);
}