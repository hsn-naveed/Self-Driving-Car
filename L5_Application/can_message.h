/// DBC file: 243.dbc    Self node: SENSOR
/// This file should be included by a source file, for example: #include "generated.c"
#ifndef CAN_MESSAGE_H__
#define CAN_MESSAGE_H__
#ifdef __cplusplus
extern "C" {
#endif

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

static const msg_hdr_t MASTER_TX_MASTER_HEARTBEAT_HDR =       {  100, 1 };
static const msg_hdr_t SENSOR_TX_SENSOR_SONARS_HDR =          {  700, 6 };
static const msg_hdr_t MASTER_TX_MOTOR_CMD_HDR =              {  704, 1 };
static const msg_hdr_t GPS_TX_GPS_INFO_COORDINATES_HDR =      {  706, 1 };
static const msg_hdr_t GPS_TX_GPS_INFO_HEADING_HDR =          {  708, 1 };
static const msg_hdr_t ANDROID_TX_INFO_COORDINATES_HDR =      {  712, 8 };
static const msg_hdr_t GPS_TX_INFO_HEADING_HDR =              {  716, 4 };
static const msg_hdr_t GPS_TX_DESTINATION_REACHED_HDR =       {  720, 1 };

/// Message: MASTER_HEARTBEAT from 'MASTER', DLC: 1 byte(s), MID: 100
typedef struct {
    uint8_t MASTER_HEARTBEAT_cmd;        ///< B7:0   Destination: SENSOR,MOTOR

    mia_info_t mia_info;
} MASTER_TX_MASTER_HEARTBEAT_t;


/// Message: SENSOR_SONARS from 'SENSOR', DLC: 6 byte(s), MID: 700
typedef struct {
    float SENSOR_SONARS_left;            ///< B11:0   Destination: MASTER
    float SENSOR_SONARS_middle;          ///< B23:12   Destination: MASTER
    float SENSOR_SONARS_right;           ///< B35:24   Destination: MASTER
    float SENSOR_SONARS_rear;            ///< B47:36   Destination: MASTER

    mia_info_t mia_info;
} SENSOR_TX_SENSOR_SONARS_t;


/// Not generating 'MASTER_TX_MOTOR_CMD_t' since we are not the sender or a recipient of any of its signals

/// Not generating 'GPS_TX_GPS_INFO_COORDINATES_t' since we are not the sender or a recipient of any of its signals

/// Not generating 'GPS_TX_GPS_INFO_HEADING_t' since we are not the sender or a recipient of any of its signals

/// These 'externs' need to be defined in a source file of your project
extern const uint32_t MASTER_HEARTBEAT__MIA_MS;
extern const MASTER_TX_MASTER_HEARTBEAT_t MASTER_HEARTBEAT__MIA_MSG;

/// Not generating code for MASTER_TX_MASTER_HEARTBEAT_encode() since the sender is MASTER and we are SENSOR

/// Encode SENSOR's 'SENSOR_SONARS' message
/// @returns the message header of this message
static msg_hdr_t SENSOR_TX_SENSOR_SONARS_encode(uint64_t *to, SENSOR_TX_SENSOR_SONARS_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->SENSOR_SONARS_left - (0)) / 0.1) + 0.5)) & 0xfff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0
    bytes[1] |= (((uint8_t)(raw_signal >> 8) & 0x0f) << 0); ///< 4 bit(s) to B8

    raw_signal = ((uint64_t)(((from->SENSOR_SONARS_middle - (0)) / 0.1) + 0.5)) & 0xfff;
    bytes[1] |= (((uint8_t)(raw_signal >> 0) & 0x0f) << 4); ///< 4 bit(s) to B12
    bytes[2] |= (((uint8_t)(raw_signal >> 4) & 0xff) << 0); ///< 8 bit(s) to B16

    raw_signal = ((uint64_t)(((from->SENSOR_SONARS_right - (0)) / 0.1) + 0.5)) & 0xfff;
    bytes[3] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B24
    bytes[4] |= (((uint8_t)(raw_signal >> 8) & 0x0f) << 0); ///< 4 bit(s) to B32

    raw_signal = ((uint64_t)(((from->SENSOR_SONARS_rear - (0)) / 0.1) + 0.5)) & 0xfff;
    bytes[4] |= (((uint8_t)(raw_signal >> 0) & 0x0f) << 4); ///< 4 bit(s) to B36
    bytes[5] |= (((uint8_t)(raw_signal >> 4) & 0xff) << 0); ///< 8 bit(s) to B40

    return SENSOR_TX_SENSOR_SONARS_HDR;
}


/// Not generating code for MASTER_TX_MOTOR_CMD_encode() since the sender is MASTER and we are SENSOR

/// Not generating code for GPS_TX_GPS_INFO_COORDINATES_encode() since the sender is GPS and we are SENSOR

/// Not generating code for GPS_TX_GPS_INFO_HEADING_encode() since the sender is GPS and we are SENSOR

/// Decode MASTER's 'MASTER_HEARTBEAT' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool MASTER_TX_MASTER_HEARTBEAT_decode(MASTER_TX_MASTER_HEARTBEAT_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != MASTER_TX_MASTER_HEARTBEAT_HDR.dlc || hdr->mid != MASTER_TX_MASTER_HEARTBEAT_HDR.mid)) {
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


/// Not generating code for SENSOR_TX_SENSOR_SONARS_decode() since we are not the recipient of any of its signals

/// Not generating code for MASTER_TX_MOTOR_CMD_decode() since we are not the recipient of any of its signals

/// Not generating code for GPS_TX_GPS_INFO_COORDINATES_decode() since we are not the recipient of any of its signals

/// Not generating code for GPS_TX_GPS_INFO_HEADING_decode() since we are not the recipient of any of its signals

/// Handle the MIA for MASTER's 'MASTER_HEARTBEAT' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool MASTER_TX_MASTER_HEARTBEAT_handle_mia(MASTER_TX_MASTER_HEARTBEAT_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= MASTER_HEARTBEAT__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = MASTER_HEARTBEAT__MIA_MSG;
        msg->mia_info.mia_counter_ms = MASTER_HEARTBEAT__MIA_MS;
        msg->mia_info.is_mia = truetx;
        mia_occurred = true;
    }

    return mia_occurred;
}

#ifdef __cplusplus
}
#endif
#endif // CAN_MESSAGE_H__
