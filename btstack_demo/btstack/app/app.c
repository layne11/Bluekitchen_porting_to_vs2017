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
	 	/*HFP EVENT*/
	case APP_EVT_HFP_CONN:
		break;
	case APP_EVT_HFP_DISCONN:
		break;
	case APP_EVT_HFP_INCOMING_CALL:
		break;
	case APP_EVT_HFP_OUTGOING_CALL:
		break;
	case APP_EVT_HFP_CALLING_DEV:
		break;
	case APP_EVT_HFP_CALLING_PHONE:
		break;
		/*A2DP EVENT*/
	case APP_EVT_A2DP_CONN:
		break;
	case APP_EVT_A2DP_DISCONN:
		break;
		/*AVRCP EVENT*/
	case APP_EVT_AVRCP_CONN:
		break;
	case APP_EVT_AVRCP_DISCONN:
		break;
		/*GATT EVENT*/
	case APP_EVT_GATT_CONN:
		break;
	case APP_EVT_GATT_DISCONN:
		break;
	case APP_EVT_GATT_DATA_RCV:
		break;
		/*SPP EVENT*/
	case APP_EVT_SPP_CONN:
		break;
	case APP_EVT_SPP_DISCONN:
		break;
	case APP_EVT_SPP_DATA_RCV:
		break;
	}
}
void btstack_evt_display_handler_regeister(btstack_evt_display_handler_t func)
{
	btstack_evt_display_handler = func;
}

void app_audio_operation_regeister(audio_operation *opt)
{
	audio_sink_out_in_operation_regeister(opt);
	sco_data_out_int_operation_regeister(opt);
}

void app_init(void)
{
	l2cap_init();
	rfcomm_init();
	sdp_init();
	gap_set_local_name("Bluetooth_test1");
	gap_discoverable_control(1);
#if defined(APP_TYPE_PERIPHERAL)
	gap_set_class_of_device(0x200408);
#elif defined(APP_TYPE_CENTRAL)
	gap_set_class_of_device(0x5a020c);
#endif

#if defined(APP_TYPE_PERIPHERAL)
	hfp_hf_evt_handler_register(btstack_evt_handler);
	audio_sink_evt_handler_register(btstack_evt_handler);
	le_server_evt_handler_register(btstack_evt_handler);
	spp_server_evt_handler_register(btstack_evt_handler);
#elif defined(APP_TYPE_CENTRAL)
#endif
}