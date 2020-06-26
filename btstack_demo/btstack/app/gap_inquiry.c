/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "gap_inquiry.c"

// *****************************************************************************
/* EXAMPLE_START(gap_inquiry): GAP Inquiry Example
 *
 * @text The Generic Access Profile (GAP) defines how Bluetooth devices discover
 * and establish a connection with each other. In this example, the application
 * discovers  surrounding Bluetooth devices and collects their Class of Device
 * (CoD), page scan mode, clock offset, and RSSI. After that, the remote name of
 * each device is requested. In the following section we outline the Bluetooth
 * logic part, i.e., how the packet handler handles the asynchronous events and
 * data packets.
 */
// *****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btstack.h"
#include "app.h"
 
#define MAX_DEVICES 100//20
enum DEVICE_STATE { REMOTE_NAME_REQUEST, REMOTE_NAME_INQUIRED, REMOTE_NAME_FETCHED, REMOTE_NAME_FETCH_FAIL};
struct device {
    bd_addr_t          address;
    uint8_t            pageScanRepetitionMode;
    uint16_t           clockOffset;
    enum DEVICE_STATE  state; 
};

#define INQUIRY_INTERVAL 5



enum STATE {INIT, ACTIVE} ;
static enum STATE gap_state = INIT;

static btstack_packet_callback_registration_t hci_event_callback_registration;


struct device devices[MAX_DEVICES];
int deviceCount = 0;
static g_scan_enable = 0;
static struct device_info gap_inquiry_devs_list[100];
static uint32_t gap_inquiry_devs_cnt = 0;
static btstack_display_scan_result_t gap_inquiry_display_result = NULL;


// returns 1 if name is found in advertisement
static int gap_inquiry_recode_dev_info(uint8_t * advertisement_report, uint16_t len)
{
	struct device_info m_dev;

	memset(&m_dev, 0, sizeof(struct device_info));
#if 0
	printf("hfp ag scan data:\n");
	for (int j = 0; j < len; j++) {
		printf("%0x ", advertisement_report[j]);
	}
	printf("\n");
#endif

	gap_event_inquiry_result_get_bd_addr(advertisement_report, m_dev.bd_addr_t);
	m_dev.dev_type = 0;

	if (gap_event_inquiry_result_get_name_available(advertisement_report)) {
		int name_len = gap_event_inquiry_result_get_name_len(advertisement_report);
		memcpy(m_dev.dev_name, gap_event_inquiry_result_get_name(advertisement_report), name_len);
		m_dev.dev_name[name_len] = '\0';
		devices[deviceCount].state = REMOTE_NAME_FETCHED;;
	} else {
		devices[deviceCount].state = REMOTE_NAME_REQUEST;
	}
	//printf("device name:%s\n", m_dev.dev_name);

	for (int i = 0; i < gap_inquiry_devs_cnt; i++) {
		int m = 0;
		for (m = 0; m < 6; m++) {
			if (gap_inquiry_devs_list[i].bd_addr_t[m] != m_dev.bd_addr_t[m])
				break;
		}
		if (6 == m && gap_inquiry_devs_list[i].bd_addr_t[5] == m_dev.bd_addr_t[5])
			return 1;
	}

	memcpy(&gap_inquiry_devs_list[gap_inquiry_devs_cnt], &m_dev, sizeof(struct device_info));

	memcpy(&devices[deviceCount].address, &m_dev.bd_addr_t, BD_ADDR_LEN);
	devices[deviceCount].pageScanRepetitionMode = gap_event_inquiry_result_get_page_scan_repetition_mode(advertisement_report);
	devices[deviceCount].clockOffset = gap_event_inquiry_result_get_clock_offset(advertisement_report);

	if (NULL != gap_inquiry_display_result)
		(*gap_inquiry_display_result)(gap_inquiry_devs_cnt, &gap_inquiry_devs_list[gap_inquiry_devs_cnt]);
	gap_inquiry_devs_cnt++;
	deviceCount++;

	return 0;
}

static int gap_inquiry_name_get(uint8_t * packet, uint16_t len)
{
	int index = -1;
	bd_addr_t addr;

	reverse_bd_addr(&packet[3], addr);
	index = getDeviceIndexForAddress(addr);
	if (index >= 0) {
		if (packet[2] == 0) {
			strcpy(gap_inquiry_devs_list[index].dev_name, &packet[9]);

			printf("Name: '%s'\n", &packet[9]);
			devices[index].state = REMOTE_NAME_FETCHED;
		} else {
			printf("Failed to get name: page timeout\n");
			//devices[index].state = REMOTE_NAME_FETCHED;
		}
	}

	if (NULL != gap_inquiry_display_result)
		(*gap_inquiry_display_result)(gap_inquiry_devs_cnt, &gap_inquiry_devs_list[index]);

}
static int getDeviceIndexForAddress( bd_addr_t addr){
    int j;
    for (j=0; j< deviceCount; j++){
        if (bd_addr_cmp(addr, devices[j].address) == 0){
            return j;
        }
    }
    return -1;
}

static void start_scan(void){
    
	if (g_scan_enable) {
		printf("Starting inquiry scan..\n");
		gap_inquiry_start(INQUIRY_INTERVAL);
	}
}

static int has_more_remote_name_requests(void){
    int i;
    for (i=0;i<deviceCount;i++) {
        if (devices[i].state == REMOTE_NAME_REQUEST) return 1;
    }
    return 0;
}

static void do_next_remote_name_request(void){
    int i;
    for (i=0;i<deviceCount;i++) {
        // remote name request
        if (devices[i].state == REMOTE_NAME_REQUEST){
            devices[i].state = REMOTE_NAME_INQUIRED;
            printf("Get remote name of %s...\n", bd_addr_to_str(devices[i].address));
            gap_remote_name_request( devices[i].address, devices[i].pageScanRepetitionMode,  devices[i].clockOffset | 0x8000);
            return;
        }
    }
}

static void continue_remote_names(void){
    if (has_more_remote_name_requests()){
        do_next_remote_name_request();
        return;
    } 
    start_scan();
}

/* @section Bluetooth Logic 
 *
 * @text The Bluetooth logic is implemented as a state machine within the packet
 * handler. In this example, the following states are passed sequentially:
 * INIT, and ACTIVE.
 */ 

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    bd_addr_t addr;
    int i;
    int index;
    
    if (packet_type != HCI_EVENT_PACKET) return;

    uint8_t event = hci_event_packet_get_type(packet);

    switch(gap_state){
        /* @text In INIT, an inquiry  scan is started, and the application transits to 
         * ACTIVE state.
         */
        case INIT:
            switch(event){
                case BTSTACK_EVENT_STATE:
                    if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
                        //start_scan();
						gap_state = ACTIVE;
                    }
                    break;
                default:
                    break;
            }
            break;

        /* @text In ACTIVE, the following events are processed:
         *  - GAP Inquiry result event: BTstack provides a unified inquiry result that contain
         *    Class of Device (CoD), page scan mode, clock offset. RSSI and name (from EIR) are optional.
         *  - Inquiry complete event: the remote name is requested for devices without a fetched 
         *    name. The state of a remote name can be one of the following: 
         *    REMOTE_NAME_REQUEST, REMOTE_NAME_INQUIRED, or REMOTE_NAME_FETCHED.
         *  - Remote name request complete event: the remote name is stored in the table and the 
         *    state is updated to REMOTE_NAME_FETCHED. The query of remote names is continued.
         */
        case ACTIVE:
            switch(event){

                case GAP_EVENT_INQUIRY_RESULT:
					if (gap_inquiry_devs_cnt >= MAX_DEVICES) break;
					gap_inquiry_recode_dev_info(packet, size);
					/*
                    if (deviceCount >= MAX_DEVICES) break;  // already full
                    gap_event_inquiry_result_get_bd_addr(packet, addr);
                    index = getDeviceIndexForAddress(addr);
                    if (index >= 0) break;   // already in our list

                    memcpy(devices[deviceCount].address, addr, 6);
                    devices[deviceCount].pageScanRepetitionMode = gap_event_inquiry_result_get_page_scan_repetition_mode(packet);
                    devices[deviceCount].clockOffset = gap_event_inquiry_result_get_clock_offset(packet);
                    // print info
                    printf("Device found: %s ",  bd_addr_to_str(addr));
                    printf("with COD: 0x%06x, ", (unsigned int) gap_event_inquiry_result_get_class_of_device(packet));
                    printf("pageScan %d, ",      devices[deviceCount].pageScanRepetitionMode);
                    printf("clock offset 0x%04x",devices[deviceCount].clockOffset);
                    if (gap_event_inquiry_result_get_rssi_available(packet)){
                        printf(", rssi %d dBm", (int8_t) gap_event_inquiry_result_get_rssi(packet));
                    }
                    if (gap_event_inquiry_result_get_name_available(packet)){
                        char name_buffer[240];
                        int name_len = gap_event_inquiry_result_get_name_len(packet);
                        memcpy(name_buffer, gap_event_inquiry_result_get_name(packet), name_len);
                        name_buffer[name_len] = 0;
                        printf(", name '%s'", name_buffer);
                        devices[deviceCount].state = REMOTE_NAME_FETCHED;;
                    } else {
                        devices[deviceCount].state = REMOTE_NAME_REQUEST;
                    }
                    printf("\n");
                    deviceCount++;*/
                    break;

                case GAP_EVENT_INQUIRY_COMPLETE:
                    for (i=0;i<deviceCount;i++) {
                        // retry remote name request
                        if (devices[i].state == REMOTE_NAME_INQUIRED)
                            devices[i].state = REMOTE_NAME_REQUEST;
                    }
                    continue_remote_names();
                    break;

                case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE:
					gap_inquiry_name_get(packet, size);
                    reverse_bd_addr(&packet[3], addr);
                    index = getDeviceIndexForAddress(addr);
                    if (index >= 0) {
                        if (packet[2] == 0) {
                            printf("Name: '%s'\n", &packet[9]);
                            devices[index].state = REMOTE_NAME_FETCHED;
                        } else {
							devices[index].state = REMOTE_NAME_FETCH_FAIL;
                            printf("Failed to get name: page timeout\n");
                        }
                    }
                    continue_remote_names();
                    break;

                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}

/* LISTING_START(MainConfiguration): Setup HFP Audio Gateway */
int app_gap_inquiry_scan_set(int scan)
{
	int ret = 0;
	gap_inquiry_devs_cnt = 0;
	deviceCount = 0;
	memset(&devices, 0, sizeof(struct device) * MAX_DEVICES);
	memset(&gap_inquiry_devs_list, 0, sizeof(struct device_info) * 100);
	g_scan_enable = scan;
	if (scan) {
		//state = TC_W4_SCAN_RESULT;
		printf("Start scanning...\n");
		ret = gap_inquiry_start(INQUIRY_INTERVAL);
	}
	else {
		//state = TC_IDLE;
		printf("Stop scanning!\n");
		ret = gap_inquiry_stop();
	}

	return ret;
}

void gap_inquiry_display_result_regeister(btstack_display_scan_result_t func)
{
	gap_inquiry_display_result = func;
}

/* @text For more details on discovering remote devices, please see
 * Section on [GAP](../profiles/#sec:GAPdiscoverRemoteDevices).
 */
/* LISTING_START(MainConfiguration): Setup packet handler for GAP inquiry */
int app_gap_inquiry_init(void)
{
    // enabled EIR
    hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);

    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // turn on!
    hci_power_control(HCI_POWER_ON);
        
    return 0;
}
/* LISTING_END */
/* EXAMPLE_END */
