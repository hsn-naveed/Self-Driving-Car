/// DBC file: 243.dbc    Self node: ANDROID
/// This file should be included by a source file, for example: #include "generated.c"
#ifndef ANDROID_MESSAGE_H__
#define ANDROID_MESSAGE_H__
#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/// Missing in Action structure
typedef struct {
    uint32_t is_mia : 1;          ///< Missing in action flag
    uint32_t mia_counter_ms : 31; ///< Missing in action counter
} mia_info_t;

/// CAN message header structure
typedef struct { 
    uint32_t mid; ///< Message ID of the message
    uint8_t  dlc; ///< Data length of the message
} msg_hdr_t; 

static const msg_hdr_t MASTER_TX_HEARTBEAT_HDR =              {  100, 1 };
static const msg_hdr_t SENSOR_TX_INFO_SONARS_HDR =            {  600, 4 };
static const msg_hdr_t MASTER_TX_MOTOR_CMD_HDR =              {  604, 2 };
static const msg_hdr_t ANDROID_TX_STOP_GO_CMD_HDR =           {  700, 1 };
static const msg_hdr_t ANDROID_TX_INFO_CHECKPOINTS_HDR =      {  708, 1 };
static const msg_hdr_t ANDROID_TX_INFO_COORDINATES_HDR =      {  712, 8 };
static const msg_hdr_t GPS_TX_INFO_HEADING_HDR =              {  716, 4 };
static const msg_hdr_t GPS_TX_DESTINATION_REACHED_HDR =       {  720, 1 };


/// Not generating 'MASTER_TX_HEARTBEAT_t' since we are not the sender or a recipient of any of its signals

/// Not generating 'SENSOR_TX_INFO_SONARS_t' since we are not the sender or a recipient of any of its signals

/// Not generating 'MASTER_TX_MOTOR_CMD_t' since we are not the sender or a recipient of any of its signals

/// Message: STOP_GO_CMD from 'ANDROID', DLC: 1 byte(s), MID: 700
typedef struct {
    uint8_t ANDROID_STOP_CMD_signal;     ///< B7:0   Destination: MASTER,GPS

    mia_info_t mia_info;
} ANDROID_TX_STOP_GO_CMD_t;


/// Message: INFO_CHECKPOINTS from 'ANDROID', DLC: 1 byte(s), MID: 708
typedef struct {
    uint8_t ANDROID_INFO_CHECKPOINTS_count; ///< B7:0  Min: 0 Max: 255   Destination: MASTER,GPS

    mia_info_t mia_info;
} ANDROID_TX_INFO_CHECKPOINTS_t;


/// Message: INFO_COORDINATES from 'ANDROID', DLC: 8 byte(s), MID: 712
typedef struct {
    float GPS_INFO_COORDINATES_lat;      ///< B31:0   Destination: MASTER,GPS
    float GPS_INFO_COORDINATES_long;     ///< B63:32   Destination: MASTER,GPS

    mia_info_t mia_info;
} ANDROID_TX_INFO_COORDINATES_t;


/// Not generating 'GPS_TX_INFO_HEADING_t' since we are not the sender or a recipient of any of its signals

/// Message: DESTINATION_REACHED from 'GPS', DLC: 1 byte(s), MID: 720
typedef struct {
    uint8_t GPS_DESTINATION_REACHED_signal; ///< B7:0   Destination: MASTER,ANDROID

    mia_info_t mia_info;
} GPS_TX_DESTINATION_REACHED_t;


/// These 'externs' need to be defined in a source file of your project
extern const uint32_t DESTINATION_REACHED__MIA_MS;
extern const GPS_TX_DESTINATION_REACHED_t DESTINATION_REACHED__MIA_MSG;

/// Not generating code for MASTER_TX_HEARTBEAT_encode() since the sender is MASTER and we are ANDROID

/// Not generating code for SENSOR_TX_INFO_SONARS_encode() since the sender is SENSOR and we are ANDROID

/// Not generating code for MASTER_TX_MOTOR_CMD_encode() since the sender is MASTER and we are ANDROID

/// Encode ANDROID's 'STOP_GO_CMD' message
/// @returns the message header of this message
static msg_hdr_t ANDROID_TX_STOP_GO_CMD_encode(uint64_t *to, ANDROID_TX_STOP_GO_CMD_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->ANDROID_STOP_CMD_signal - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0

    return ANDROID_TX_STOP_GO_CMD_HDR;
}


/// Encode ANDROID's 'INFO_CHECKPOINTS' message
/// @returns the message header of this message
static msg_hdr_t ANDROID_TX_INFO_CHECKPOINTS_encode(uint64_t *to, ANDROID_TX_INFO_CHECKPOINTS_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    if(from->ANDROID_INFO_CHECKPOINTS_count < 0) { from->ANDROID_INFO_CHECKPOINTS_count = 0; }
    if(from->ANDROID_INFO_CHECKPOINTS_count > 255) { from->ANDROID_INFO_CHECKPOINTS_count = 255; }
    raw_signal = ((uint64_t)(((from->ANDROID_INFO_CHECKPOINTS_count - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0

    return ANDROID_TX_INFO_CHECKPOINTS_HDR;
}


/// Encode ANDROID's 'INFO_COORDINATES' message
/// @returns the message header of this message
static msg_hdr_t ANDROID_TX_INFO_COORDINATES_encode(uint64_t *to, ANDROID_TX_INFO_COORDINATES_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->GPS_INFO_COORDINATES_lat - (0)) / 1e-05) + 0.5)) & 0xffffffff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0
    bytes[1] |= (((uint8_t)(raw_signal >> 8) & 0xff) << 0); ///< 8 bit(s) to B8
    bytes[2] |= (((uint8_t)(raw_signal >> 16) & 0xff) << 0); ///< 8 bit(s) to B16
    bytes[3] |= (((uint8_t)(raw_signal >> 24) & 0xff) << 0); ///< 8 bit(s) to B24

    raw_signal = ((uint64_t)(((from->GPS_INFO_COORDINATES_long - (0)) / 1e-05) + 0.5)) & 0xffffffff;
    bytes[4] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B32
    bytes[5] |= (((uint8_t)(raw_signal >> 8) & 0xff) << 0); ///< 8 bit(s) to B40
    bytes[6] |= (((uint8_t)(raw_signal >> 16) & 0xff) << 0); ///< 8 bit(s) to B48
    bytes[7] |= (((uint8_t)(raw_signal >> 24) & 0xff) << 0); ///< 8 bit(s) to B56

    return ANDROID_TX_INFO_COORDINATES_HDR;
}


/// Not generating code for GPS_TX_INFO_HEADING_encode() since the sender is GPS and we are ANDROID

/// Not generating code for GPS_TX_DESTINATION_REACHED_encode() since the sender is GPS and we are ANDROID

/// Not generating code for MASTER_TX_HEARTBEAT_decode() since we are not the recipient of any of its signals

/// Not generating code for SENSOR_TX_INFO_SONARS_decode() since we are not the recipient of any of its signals

/// Not generating code for MASTER_TX_MOTOR_CMD_decode() since we are not the recipient of any of its signals

/// Not generating code for ANDROID_TX_STOP_GO_CMD_decode() since we are not the recipient of any of its signals

/// Not generating code for ANDROID_TX_INFO_CHECKPOINTS_decode() since we are not the recipient of any of its signals

/// Not generating code for ANDROID_TX_INFO_COORDINATES_decode() since we are not the recipient of any of its signals

/// Not generating code for GPS_TX_INFO_HEADING_decode() since we are not the recipient of any of its signals

/// Decode GPS's 'DESTINATION_REACHED' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool GPS_TX_DESTINATION_REACHED_decode(GPS_TX_DESTINATION_REACHED_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != GPS_TX_DESTINATION_REACHED_HDR.dlc || hdr->mid != GPS_TX_DESTINATION_REACHED_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->GPS_DESTINATION_REACHED_signal = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Handle the MIA for GPS's 'DESTINATION_REACHED' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool GPS_TX_DESTINATION_REACHED_handle_mia(GPS_TX_DESTINATION_REACHED_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= DESTINATION_REACHED__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = DESTINATION_REACHED__MIA_MSG;
        msg->mia_info.mia_counter_ms = DESTINATION_REACHED__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

#ifdef __cplusplus
}
#endif
#endif
