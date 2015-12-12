/// DBC file: 243.dbc    Self node: MASTER
/// This file should be included by a source file, for example: #include "generated.c"

#ifndef CAN_MESSAGE_H__
#define CAN_MESSAGE_H__
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


/// Message: HEARTBEAT from 'MASTER', DLC: 1 byte(s), MID: 100
typedef struct {
    uint8_t MASTER_HEARTBEAT_cmd;        ///< B7:0   Destination: SENSOR,MOTOR

    mia_info_t mia_info;
} MASTER_TX_HEARTBEAT_t;


/// Message: INFO_SONARS from 'SENSOR', DLC: 4 byte(s), MID: 600
typedef struct {
    uint8_t SENSOR_INFO_SONARS_left;     ///< B7:0  Min: 0 Max: 255   Destination: MASTER
    uint8_t SENSOR_INFO_SONARS_middle;   ///< B15:8  Min: 0 Max: 255   Destination: MASTER
    uint8_t SENSOR_INFO_SONARS_right;    ///< B23:16  Min: 0 Max: 255   Destination: MASTER
    uint8_t SENSOR_INFO_SONARS_rear;     ///< B31:24  Min: 0 Max: 255   Destination: MASTER

    mia_info_t mia_info;
} SENSOR_TX_INFO_SONARS_t;


/// Message: MOTOR_CMD from 'MASTER', DLC: 2 byte(s), MID: 604
typedef struct {
    uint8_t MASTER_MOTOR_CMD_steer;      ///< B7:0  Min: 0 Max: 20   Destination: MOTOR
    uint8_t MASTER_MOTOR_CMD_drive;      ///< B15:8  Min: 0 Max: 20   Destination: MOTOR

    mia_info_t mia_info;
} MASTER_TX_MOTOR_CMD_t;


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


/// Message: INFO_HEADING from 'GPS', DLC: 4 byte(s), MID: 716
typedef struct {
    int32_t GPS_INFO_HEADING_current;    ///< B15:0   Destination: MASTER
    int32_t GPS_INFO_HEADING_dst;        ///< B31:16   Destination: MASTER

    mia_info_t mia_info;
} GPS_TX_INFO_HEADING_t;


/// Message: DESTINATION_REACHED from 'GPS', DLC: 1 byte(s), MID: 720
typedef struct {
    uint8_t GPS_DESTINATION_REACHED_signal; ///< B7:0   Destination: MASTER,ANDROID

    mia_info_t mia_info;
} GPS_TX_DESTINATION_REACHED_t;


/// These 'externs' need to be defined in a source file of your project
extern const uint32_t INFO_SONARS__MIA_MS;
extern const SENSOR_TX_INFO_SONARS_t INFO_SONARS__MIA_MSG;
extern const uint32_t STOP_GO_CMD__MIA_MS;
extern const ANDROID_TX_STOP_GO_CMD_t STOP_GO_CMD__MIA_MSG;
extern const uint32_t INFO_CHECKPOINTS__MIA_MS;
extern const ANDROID_TX_INFO_CHECKPOINTS_t INFO_CHECKPOINTS__MIA_MSG;
extern const uint32_t INFO_COORDINATES__MIA_MS;
extern const ANDROID_TX_INFO_COORDINATES_t INFO_COORDINATES__MIA_MSG;
extern const uint32_t INFO_HEADING__MIA_MS;
extern const GPS_TX_INFO_HEADING_t INFO_HEADING__MIA_MSG;
extern const uint32_t DESTINATION_REACHED__MIA_MS;
extern const GPS_TX_DESTINATION_REACHED_t DESTINATION_REACHED__MIA_MSG;

/// Encode MASTER's 'HEARTBEAT' message
/// @returns the message header of this message
static msg_hdr_t MASTER_TX_HEARTBEAT_encode(uint64_t *to, MASTER_TX_HEARTBEAT_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->MASTER_HEARTBEAT_cmd - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0

    return MASTER_TX_HEARTBEAT_HDR;
}


/// Not generating code for SENSOR_TX_INFO_SONARS_encode() since the sender is SENSOR and we are MASTER

/// Encode MASTER's 'MOTOR_CMD' message
/// @returns the message header of this message
static msg_hdr_t MASTER_TX_MOTOR_CMD_encode(uint64_t *to, MASTER_TX_MOTOR_CMD_t *from)
{
   // *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    if(from->MASTER_MOTOR_CMD_steer < 0) { from->MASTER_MOTOR_CMD_steer = 0; }
    if(from->MASTER_MOTOR_CMD_steer > 20) { from->MASTER_MOTOR_CMD_steer = 20; }
    raw_signal = ((uint64_t)(((from->MASTER_MOTOR_CMD_steer - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0

    if(from->MASTER_MOTOR_CMD_drive < 0) { from->MASTER_MOTOR_CMD_drive = 0; }
    if(from->MASTER_MOTOR_CMD_drive > 20) { from->MASTER_MOTOR_CMD_drive = 20; }
    raw_signal = ((uint64_t)(((from->MASTER_MOTOR_CMD_drive - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[1] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B8

    return MASTER_TX_MOTOR_CMD_HDR;
}


/// Not generating code for ANDROID_TX_STOP_GO_CMD_encode() since the sender is ANDROID and we are MASTER

/// Not generating code for ANDROID_TX_INFO_CHECKPOINTS_encode() since the sender is ANDROID and we are MASTER

/// Not generating code for ANDROID_TX_INFO_COORDINATES_encode() since the sender is ANDROID and we are MASTER

/// Not generating code for GPS_TX_INFO_HEADING_encode() since the sender is GPS and we are MASTER

/// Not generating code for GPS_TX_DESTINATION_REACHED_encode() since the sender is GPS and we are MASTER

/// Not generating code for MASTER_TX_HEARTBEAT_decode() since we are not the recipient of any of its signals

/// Decode SENSOR's 'INFO_SONARS' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool SENSOR_TX_INFO_SONARS_decode(SENSOR_TX_INFO_SONARS_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != SENSOR_TX_INFO_SONARS_HDR.dlc || hdr->mid != SENSOR_TX_INFO_SONARS_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->SENSOR_INFO_SONARS_left = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[1] >> 0) & 0xff); ///< 8 bit(s) from B8
    raw_signal    |= (bits_from_byte << 0);
    to->SENSOR_INFO_SONARS_middle = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[2] >> 0) & 0xff); ///< 8 bit(s) from B16
    raw_signal    |= (bits_from_byte << 0);
    to->SENSOR_INFO_SONARS_right = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[3] >> 0) & 0xff); ///< 8 bit(s) from B24
    raw_signal    |= (bits_from_byte << 0);
    to->SENSOR_INFO_SONARS_rear = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Not generating code for MASTER_TX_MOTOR_CMD_decode() since we are not the recipient of any of its signals

/// Decode ANDROID's 'STOP_GO_CMD' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool ANDROID_TX_STOP_GO_CMD_decode(ANDROID_TX_STOP_GO_CMD_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != ANDROID_TX_STOP_GO_CMD_HDR.dlc || hdr->mid != ANDROID_TX_STOP_GO_CMD_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->ANDROID_STOP_CMD_signal = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Decode ANDROID's 'INFO_CHECKPOINTS' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool ANDROID_TX_INFO_CHECKPOINTS_decode(ANDROID_TX_INFO_CHECKPOINTS_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != ANDROID_TX_INFO_CHECKPOINTS_HDR.dlc || hdr->mid != ANDROID_TX_INFO_CHECKPOINTS_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0) & 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->ANDROID_INFO_CHECKPOINTS_count = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Decode ANDROID's 'INFO_COORDINATES' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool ANDROID_TX_INFO_COORDINATES_decode(ANDROID_TX_INFO_COORDINATES_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != ANDROID_TX_INFO_COORDINATES_HDR.dlc || hdr->mid != ANDROID_TX_INFO_COORDINATES_HDR.mid)) {
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
    bits_from_byte = ((bytes[2] >> 0) & 0xff); ///< 8 bit(s) from B16
    raw_signal    |= (bits_from_byte << 16);
    bits_from_byte = ((bytes[3] >> 0) & 0xff); ///< 8 bit(s) from B24
    raw_signal    |= (bits_from_byte << 24);
    to->GPS_INFO_COORDINATES_lat = (raw_signal * 1e-05) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[4] >> 0) & 0xff); ///< 8 bit(s) from B32
    raw_signal    |= (bits_from_byte << 0);
    bits_from_byte = ((bytes[5] >> 0) & 0xff); ///< 8 bit(s) from B40
    raw_signal    |= (bits_from_byte << 8);
    bits_from_byte = ((bytes[6] >> 0) & 0xff); ///< 8 bit(s) from B48
    raw_signal    |= (bits_from_byte << 16);
    bits_from_byte = ((bytes[7] >> 0) & 0xff); ///< 8 bit(s) from B56
    raw_signal    |= (bits_from_byte << 24);
    to->GPS_INFO_COORDINATES_long = (raw_signal * 1e-05) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


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


/// Handle the MIA for SENSOR's 'INFO_SONARS' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool SENSOR_TX_INFO_SONARS_handle_mia(SENSOR_TX_INFO_SONARS_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= INFO_SONARS__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = INFO_SONARS__MIA_MSG;
        msg->mia_info.mia_counter_ms = INFO_SONARS__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for ANDROID's 'STOP_GO_CMD' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool ANDROID_TX_STOP_GO_CMD_handle_mia(ANDROID_TX_STOP_GO_CMD_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= STOP_GO_CMD__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = STOP_GO_CMD__MIA_MSG;
        msg->mia_info.mia_counter_ms = STOP_GO_CMD__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for ANDROID's 'INFO_CHECKPOINTS' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool ANDROID_TX_INFO_CHECKPOINTS_handle_mia(ANDROID_TX_INFO_CHECKPOINTS_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= INFO_CHECKPOINTS__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = INFO_CHECKPOINTS__MIA_MSG;
        msg->mia_info.mia_counter_ms = INFO_CHECKPOINTS__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for ANDROID's 'INFO_COORDINATES' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool ANDROID_TX_INFO_COORDINATES_handle_mia(ANDROID_TX_INFO_COORDINATES_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= INFO_COORDINATES__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = INFO_COORDINATES__MIA_MSG;
        msg->mia_info.mia_counter_ms = INFO_COORDINATES__MIA_MS;
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
#endif /* CAN_MESSAGE_H__ */
