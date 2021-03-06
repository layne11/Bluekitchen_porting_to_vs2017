//
// btstack_config.h for libusb port
//

#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

// Port related features
#define HAVE_MALLOC
//#define HAVE_POSIX_FILE_IO
//#define HAVE_BTSTACK_STDIN
#define HAVE_POSIX_TIME

// BTstack features that can be enabled
#define ENABLE_BLE
#define ENABLE_CLASSIC
#define ENABLE_HFP_WIDE_BAND_SPEECH
#define ENABLE_LE_CENTRAL
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LE_SECURE_CONNECTIONS
#define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS
#define ENABLE_LE_DATA_CHANNELS
#define ENABLE_LE_DATA_LENGTH_EXTENSION
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_INFO 
#define ENABLE_SCO_OVER_HCI
#define ENABLE_SDP_DES_DUMP

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE (1691 + 4)
#define HCI_INCOMING_PRE_BUFFER_SIZE 14 // sizeof BNEP header, avoid memcpy


#define NVM_NUM_LINK_KEYS 16
#define NVM_NUM_DEVICE_DB_ENTRIES 1//16


#define ENABLE_ATT_DELAYED_RESPONSE
#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE

/*add by layne*/
//#define APP_TYPE_CENTRAL
#define APP_TYPE_PERIPHERAL

#if defined(APP_TYPE_PERIPHERAL)
#define APP_HFP_HF
#define APP_AUDIO_SINK
#define APP_LE_STREAMER_SERVER
#define APP_SPP_STREAMER_SERVER
#elif defined(APP_TYPE_CENTRAL)
#define APP_HFP_AG
#define APP_AUDIO_SOURCE
#define APP_LE_STREAMER_CLIENT
#define APP_SPP_STREAMER_CLIENT
#endif

#endif
