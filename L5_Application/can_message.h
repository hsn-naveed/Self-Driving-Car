#ifndef CAN_MESSAGE_H__
#define CAN_MESSAGE_H__
#ifdef __cplusplus
extern "C" {
#endif
/// DBC file: 243.dbc    Self node: MOTOR
/// This file should be included by a source file, for example: #include "generated.c"
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


/// Message: HEARTBEAT from 'MASTER', DLC: 1 byte(s), MID: 100
typedef struct {
        uint8_t MASTER_HEARTBEAT_cmd;        ///< B7:0   Destination: SENSOR,MOTOR

        mia_info_t mia_info;
} MASTER_TX_HEARTBEAT_t;


/// Not generating 'SENSOR_TX_INFO_SONARS_t' since we are not the sender or a recipient of any of its signals

/// Message: MOTOR_CMD from 'MASTER', DLC: 2 byte(s), MID: 604
typedef struct {
        uint8_t MASTER_MOTOR_CMD_steer;      ///< B7:0  Min: 0 Max: 20   Destination: MOTOR
        uint8_t MASTER_MOTOR_CMD_drive;      ///< B15:8  Min: 0 Max: 20   Destination: MOTOR

        mia_info_t mia_info;
} MASTER_TX_MOTOR_CMD_t;


/// Not generating 'ANDROID_TX_STOP_GO_CMD_t' since we are not the sender or a recipient of any of its signals

/// Not generating 'ANDROID_TX_INFO_CHECKPOINTS_t' since we are not the sender or a recipient of any of its signals

/// Not generating 'ANDROID_TX_INFO_COORDINATES_t' since we are not the sender or a recipient of any of its signals

/// Message: INFO_HEADING from 'GPS', DLC: 4 byte(s), MID: 716
typedef struct {
        int32_t GPS_INFO_HEADING_current;    ///< B15:0   Destination: MASTER,MOTOR
        int32_t GPS_INFO_HEADING_dst;        ///< B31:16   Destination: MASTER,MOTOR

        mia_info_t mia_info;
} GPS_TX_INFO_HEADING_t;


/// Not generating 'GPS_TX_DESTINATION_REACHED_t' since we are not the sender or a recipient of any of its signals

/// These 'externs' need to be defined in a source file of your project
extern const uint32_t HEARTBEAT__MIA_MS;
extern const MASTER_TX_HEARTBEAT_t HEARTBEAT__MIA_MSG;
extern const uint32_t MOTOR_CMD__MIA_MS;
extern const MASTER_TX_MOTOR_CMD_t MOTOR_CMD__MIA_MSG;
extern const uint32_t INFO_HEADING__MIA_MS;
extern const GPS_TX_INFO_HEADING_t INFO_HEADING__MIA_MSG;

/// Not generating code for MASTER_TX_HEARTBEAT_encode() since the sender is MASTER and we are MOTOR

/// Not generating code for SENSOR_TX_INFO_SONARS_encode() since the sender is SENSOR and we are MOTOR

/// Not generating code for MASTER_TX_MOTOR_CMD_encode() since the sender is MASTER and we are MOTOR

/// Not generating code for ANDROID_TX_STOP_GO_CMD_encode() since the sender is ANDROID and we are MOTOR

/// Not generating code for ANDROID_TX_INFO_CHECKPOINTS_encode() since the sender is ANDROID and we are MOTOR

/// Not generating code for ANDROID_TX_INFO_COORDINATES_encode() since the sender is ANDROID and we are MOTOR

/// Not generating code for GPS_TX_INFO_HEADING_encode() since the sender is GPS and we are MOTOR

/// Not generating code for GPS_TX_DESTINATION_REACHED_encode() since the sender is GPS and we are MOTOR

/// Decode MASTER's 'HEARTBEAT' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool MASTER_TX_HEARTBEAT_decode(MASTER_TX_HEARTBEAT_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != MASTER_TX_HEARTBEAT_HDR.dlc || hdr->mid != MASTER_TX_HEARTBEAT_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->MASTER_HEARTBEAT_cmd = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Not generating code for SENSOR_TX_INFO_SONARS_decode() since we are not the recipient of any of its signals

/// Decode MASTER's 'MOTOR_CMD' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool MASTER_TX_MOTOR_CMD_decode(MASTER_TX_MOTOR_CMD_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != MASTER_TX_MOTOR_CMD_HDR.dlc || hdr->mid != MASTER_TX_MOTOR_CMD_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->MASTER_MOTOR_CMD_steer = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[1] >> 0) & 0xff); ///< 8 bit(s) from B8
    raw_signal    |= (bits_from_byte << 0);
    to->MASTER_MOTOR_CMD_drive = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Not generating code for ANDROID_TX_STOP_GO_CMD_decode() since we are not the recipient of any of its signals

/// Not generating code for ANDROID_TX_INFO_CHECKPOINTS_decode() since we are not the recipient of any of its signals

/// Not generating code for ANDROID_TX_INFO_COORDINATES_decode() since we are not the recipient of any of its signals

/// Decode GPS's 'INFO_HEADING' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool GPS_TX_INFO_HEADING_decode(GPS_TX_INFO_HEADING_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != GPS_TX_INFO_HEADING_HDR.dlc || hdr->mid != GPS_TX_INFO_HEADING_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    bits_from_byte = ((bytes[1] >> 0) & 0xff); ///< 8 bit(s) from B8
    raw_signal    |= (bits_from_byte << 8);
    to->GPS_INFO_HEADING_current = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[2] >> 0) & 0xff); ///< 8 bit(s) from B16
    raw_signal    |= (bits_from_byte << 0);
    bits_from_byte = ((bytes[3] >> 0) & 0xff); ///< 8 bit(s) from B24
    raw_signal    |= (bits_from_byte << 8);
    to->GPS_INFO_HEADING_dst = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Not generating code for GPS_TX_DESTINATION_REACHED_decode() since we are not the recipient of any of its signals

/// Handle the MIA for MASTER's 'HEARTBEAT' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool MASTER_TX_HEARTBEAT_handle_mia(MASTER_TX_HEARTBEAT_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= HEARTBEAT__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = HEARTBEAT__MIA_MSG;
        msg->mia_info.mia_counter_ms = HEARTBEAT__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for MASTER's 'MOTOR_CMD' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool MASTER_TX_MOTOR_CMD_handle_mia(MASTER_TX_MOTOR_CMD_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= MOTOR_CMD__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = MOTOR_CMD__MIA_MSG;
        msg->mia_info.mia_counter_ms = MOTOR_CMD__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for GPS's 'INFO_HEADING' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool GPS_TX_INFO_HEADING_handle_mia(GPS_TX_INFO_HEADING_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= INFO_HEADING__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = INFO_HEADING__MIA_MSG;
        msg->mia_info.mia_counter_ms = INFO_HEADING__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

#ifdef __cplusplus
}
#endif
#endif // CAN_MESSAGE_H__
