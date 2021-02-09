/*
 * Copyright 2021 Jetperch LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 * @file
 *
 * @brief JLS file format.
 */

#ifndef JLS_FORMAT_H__
#define JLS_FORMAT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup jls
 * @defgroup jls_format File format
 *
 * @brief JLS file format.
 *
 * @{
 */


#define JLS_FORMAT_VERSION_MAJOR  (0)
#define JLS_FORMAT_VERSION_MINOR  (1)
#define JLS_FORMAT_VERSION_PATCH  (1)
#define JLS_FORMAT_VERSION_U32     ((uint32_t) ( \
    ((JLS_FORMAT_VERSION_MAJOR & 0xff) << 24) | \
    ((JLS_FORMAT_VERSION_MINOR & 0xff) << 16) | \
    (JLS_FORMAT_VERSION_PATCH & 0xffff) ))

/**
 * @brief The file identification bytes at the start of the file.
 *
 * These bytes are not arbitrary.  We carefully selected them to provide:
 * - Identification: Help the application determine that
 *   this file is in the correct format with minimal uncertainty.
 * - Correct endianness:  Little endian has won, so this entire format is
 *   stored in little endian format.
 * - Proper binary processing:  The different line ending combinations
 *   ensure that the reader is not "fixing" the line endings, since this
 *   is a binary file format.
 * - Display: Include "substitute" and "file separator" so that text
 *   printers to not show the rest of the file.
 *
 * The following table describes maps each byte to its purpose:
 * | Value (hex)       | Purpose                                             |
 * | ----------------- | --------------------------------------------------- |
 * | 6A 6C 73 66 6d 74 | ASCII "jlsfmt", when viewed in text editor.         |
 * | 0D 0A             | DOS line ending to ensure binary correctness.       |
 * | 20                | ASCII space.                                        |
 * | 0A                | UNIX line ending to ensure binary correctness.      |
 * | 20                | ASCII space.                                        |
 * | 1A                | Substitute character (stops listing under Windows). |
 * | 20 20             | ASCII spaces.                                       |
 * | B2                | Ensure that system supports 8-bit data              |
 * | 1C                | File separator.                                     |
 */
#define JLS_HEADER_IDENTIFICATION \
    {0x6a, 0x6c, 0x73, 0x66, 0x6d, 0x74, 0x0d, 0x0a, \
     0x20, 0x0a, 0x20, 0x1a, 0x20, 0x20, 0xb2, 0x1c}

/**
 * @brief The maximum allowed number of signals.
 */
#define JLS_SIGNAL_COUNT (256)

/**
 * @brief The number of summary levels.
 */
#define JLS_SUMMARY_LEVEL_COUNT (16)


/**
 * @brief The signal type definition.
 */
enum jls_signal_type_e {
    /// Fixed sampling rate
    JLS_SIGNAL_TYPE_FSR = 0,
    /// Variable sampling rate
    JLS_SIGNAL_TYPE_VSR = 1,
};

/**
 * @brief The available track types that store data over time.
 */
enum jls_track_type_e {
    /**
     * @brief Block tracks contain fixed sample-rate (FSR) data.
     *
     * The JLS_TAG_SIGNAL_DEF defines the sampling rate.
     */
    JLS_TRACK_TYPE_FSR = 0,

    /**
     * @brief Fixed-type, variable-sample-rate (VSR) time series data.
     *
     * Each entry consists of time in UTC and the data.
     */
    JLS_TRACK_TYPE_VSR = 1,

    /**
     * @brief Annotations contain infrequent, variable-typed data.
     *
     * Each annotation entry consists of time and the data.
     * For signals, time must be samples_id.
     * For time series, time must be UTC.
     * Annotations include UTF-8 text and markers.
     */
    JLS_TRACK_TYPE_ANNOTATION = 2,

    /**
     * @brief The UTC track associates sample_id with UTC.
     *
     * Each entry consists of sample_id, UTC pairs.  This track
     * is only used for
     */
    JLS_TRACK_TYPE_UTC = 3,
};

/**
 * @brief The chunks used to store the track information.
 */
enum jls_track_chunk_e {
    JLS_TRACK_CHUNK_DEF = 0,        // track definition
    JLS_TRACK_CHUNK_HEAD = 1,       // first INDEX chunk offset for each level (16 total)
    JLS_TRACK_CHUNK_INDEX = 2,      // The indices of all contributing summary chunks, followed immediately by summary
    JLS_TRACK_CHUNK_DATA = 3,       // sample data
    JLS_TRACK_CHUNK_SUMMARY = 4,    // reduction mean, min, max, std
};

/**
 * @brief The tag definitions.
 */
enum jls_tag_e {
    JLS_TAG_INVALID             = 0x00,

    // file definition tags
    JLS_TAG_SOURCE_DEF          = 0x01,
    JLS_TAG_SIGNAL_DEF          = 0x02,

    // track tags
    JLS_TAG_FSR_DEF             = (0x20 | (JLS_TRACK_TYPE_FSR << 3) | JLS_TRACK_CHUNK_DEF),
    JLS_TAG_FSR_HEAD            = (0x20 | (JLS_TRACK_TYPE_FSR << 3) | JLS_TRACK_CHUNK_HEAD),
    JLS_TAG_FSR_INDEX           = (0x20 | (JLS_TRACK_TYPE_FSR << 3) | JLS_TRACK_CHUNK_INDEX),
    JLS_TAG_FSR_DATA            = (0x20 | (JLS_TRACK_TYPE_FSR << 3) | JLS_TRACK_CHUNK_DATA),
    JLS_TAG_FSR_SUMMARY         = (0x20 | (JLS_TRACK_TYPE_FSR << 3) | JLS_TRACK_CHUNK_SUMMARY),

    JLS_TAG_VSR_DEF             = (0x20 | (JLS_TRACK_TYPE_VSR << 3) | JLS_TRACK_CHUNK_DEF),
    JLS_TAG_VSR_HEAD            = (0x20 | (JLS_TRACK_TYPE_VSR << 3) | JLS_TRACK_CHUNK_HEAD),
    JLS_TAG_VSR_INDEX           = (0x20 | (JLS_TRACK_TYPE_VSR << 3) | JLS_TRACK_CHUNK_INDEX),
    JLS_TAG_VSR_DATA            = (0x20 | (JLS_TRACK_TYPE_VSR << 3) | JLS_TRACK_CHUNK_DATA),
    JLS_TAG_VSR_SUMMARY         = (0x20 | (JLS_TRACK_TYPE_VSR << 3) | JLS_TRACK_CHUNK_SUMMARY),

    JLS_TAG_ANNOTATION_DEF      = (0x20 | (JLS_TRACK_TYPE_ANNOTATION << 3) | JLS_TRACK_CHUNK_DEF),  // not used
    JLS_TAG_ANNOTATION_HEAD     = (0x20 | (JLS_TRACK_TYPE_ANNOTATION << 3) | JLS_TRACK_CHUNK_HEAD),
    JLS_TAG_ANNOTATION_INDEX    = (0x20 | (JLS_TRACK_TYPE_ANNOTATION << 3) | JLS_TRACK_CHUNK_INDEX),
    JLS_TAG_ANNOTATION_DATA     = (0x20 | (JLS_TRACK_TYPE_ANNOTATION << 3) | JLS_TRACK_CHUNK_DATA),
    JLS_TAG_ANNOTATION_SUMMARY  = (0x20 | (JLS_TRACK_TYPE_ANNOTATION << 3) | JLS_TRACK_CHUNK_SUMMARY),

    JLS_TAG_UTC_DEF             = (0x20 | (JLS_TRACK_TYPE_UTC << 3) | JLS_TRACK_CHUNK_DEF),
    JLS_TAG_UTC_HEAD            = (0x20 | (JLS_TRACK_TYPE_UTC << 3) | JLS_TRACK_CHUNK_HEAD),
    JLS_TAG_UTC_INDEX           = (0x20 | (JLS_TRACK_TYPE_UTC << 3) | JLS_TRACK_CHUNK_INDEX),
    JLS_TAG_UTC_DATA            = (0x20 | (JLS_TRACK_TYPE_UTC << 3) | JLS_TRACK_CHUNK_DATA),
    JLS_TAG_UTC_SUMMARY         = (0x20 | (JLS_TRACK_TYPE_UTC << 3) | JLS_TRACK_CHUNK_SUMMARY),

    // other tags
    JLS_TAG_USER_DATA           = 0x40,
};

#define JLS_DATATYPE_BASETYPE_INT        (0x01)
#define JLS_DATATYPE_BASETYPE_UNSIGNED   (0x02)
#define JLS_DATATYPE_BASETYPE_UINT       (JLS_DATATYPE_INT | JLS_DATATYPE_UNSIGNED)
#define JLS_DATATYPE_BASETYPE_FLOAT      (0x04)

/**
 * @brief Construct a JLS datatype.
 *
 * @param basetype The datatype base type, one of [INT, UINT, FLOAT, BOOL]
 * @param size The size in bits.  Only the following options are supported:
 *      - INT: 4 * N where N is between 1 and 16, inclusive.
 *      - UINT: 1 (bool) or 4 * N where N is between 1 and 16, inclusive.
 *      - FLOAT: 32, 64
 * @param q The fixed-point location, only valid for INT and UINT.
 *      Set to 0 for normal, whole numbers.
 *      Set to 0 for FLOAT and BOOL.
 */
#define JLS_DATATYPE_DEF(basetype, size, q)     \
    ((JLS_DATATYPE_BASETYPE_##basetype) & 0x0f) |        \
    (((uint32_t) ((size) & 0xff) << 8) |   \
    (((uint32_t) (q) & 0xff) << 16)

#define JLS_DATATYPE_I32 JLS_DATATYPE_DEF(INT, 32, 0)
#define JLS_DATATYPE_I64 JLS_DATATYPE_DEF(INT, 64, 0)
#define JLS_DATATYPE_U32 JLS_DATATYPE_DEF(UINT, 32, 0)
#define JLS_DATATYPE_U64 JLS_DATATYPE_DEF(UINT, 64, 0)
#define JLS_DATATYPE_F32 JLS_DATATYPE_DEF(FLOAT, 32, 0)
#define JLS_DATATYPE_F64 JLS_DATATYPE_DEF(FLOAT, 64, 0)
#define JLS_DATATYPE_BOOL JLS_DATATYPE_DEF(UINT, 1, 0)

struct jls_source_def_s {
    uint8_t source_id;          // 0 reserved for global annotations, must be unique
    const char * name;
    const char * vendor;
    const char * model;
    const char * version;
    const char * serial_number;
};

struct jls_signal_def_base_s {
    uint8_t source_id;          // must match a source_def
    uint8_t signal_type;        // jls_signal_type_e
    uint16_t signal_id;         // must be unique
    uint32_t data_type;
    const char * name;
    const char * si_units;
};

struct jls_fsr_def_s {
    uint32_t sample_rate;
    uint32_t samples_per_block;
    uint32_t summary_downsample;
};

struct jls_vsr_def_s {
    uint32_t samples_per_block;
    uint32_t summary_downsample;
    uint8_t utc_id;
};

struct jls_signal_def_s {       // 0 reserved for VSR annotations
    struct jls_signal_def_base_s def;
    union {
        struct jls_fsr_def_s fsr;
        struct jls_vsr_def_s vsr;
    };
};

// struct jls_annotation_def_s, not used

struct jls_utc_def_s {
    uint8_t utc_id;         // 0 reserved for local computer time
    const char * name;
};

struct jls_head_s {
    uint64_t offset[16];  // 0 = data, 1 = first summary, ...
};

// struct jls_index_s variable sized, format defined by track
// struct jls_data_s variable sized, format defined by track
// struct jls_summary_s variable sized, format defined by track

// struct jls_user_data_s contains raw data.  header field chuck_meta is user-defined.

union jls_version_u {
    uint32_t u32;
    struct {
        uint16_t patch;
        uint8_t minor;
        uint8_t major;
    } s;
};

/**
 * @brief The JLS file header structure.
 */
struct jls_file_header_s {
    /**
     * @brief The semi-unique file identification header.
     *
     * @see JLS_HEADER_IDENTIFICATION
     */
    uint8_t identification[16];
    
    /**
     * @brief The file length in bytes.
     *
     * This is the last field updated on file close.  If the file was not
     * closed gracefully, the value will be 0.
     */
    uint64_t length;
    
    /**
     * @brief The JLS file format version number.
     *
     * @see JLS_FORMAT_VERSION_U32
     */
    union jls_version_u version;
    
    /**
     * @brief The CRC32 from the start of the file through version.
     */
    uint32_t crc32;
};

/**
 * @brief The JLS chunk header structure.
 *
 * Every chunk starts with this header.  This header identifies the chunk's
 * payload, and enables the chunk to participate in one doubly-linked list.
 * 
 * If the length is zero, then the next chuck header immediately follows
 * this chunk header.  If the length is not zero, then the chunk consists of:
 * - A chunk header
 * - payload of length bytes
 * - Zero padding of 0-7 bytes, so that the entire chunk will end on a mulitple
 *   of 8 bytes.  This field ends on: 8 * k - 4
 * - crc32 over the payload.
 */
struct jls_chunk_header_s {
    /**
     * @brief The next item.
     *
     * Many tags, such as CHUNK, are connected as a doubly-linked list.
     * This field indicates the location for the next item in the list.
     * The value is relative to start of the file.
     * 0 indicates end of list.
     *
     * This field allows simple, linear traversal of data, but the
     * next chunk is not known when the chunk is first created.
     * Therefore, this field requires that chunk headers are updated
     * when the software writes the next item to the file.
     */
    uint64_t item_next;

    /**
     * @brief The previous item.
     *
     * Many tags, such as CHUNK, are connected as a doubly-linked list.
     * This field indicates the location for the previous item in the list.
     * The value is relative to start of the file.
     * 0 indicates start of list.
     */
    uint64_t item_prev;

    /**
     * @brief The tag.
     *
     * The jls_tag_e value that identifies the contents of this chunk.
     */
    uint8_t tag;
    
    /// Reserved for future use.  (compression?)
    uint8_t rsv0_u8;

    /**
     * @brief The metadata associated with this chunk.
     *
     * Each tag is free to define the purpose of this field.
     * 
     * However, all data tags use this definition:
     * - tag_id[7:0] is the signal/time series identifier from 0 to 255.
     * - tag_id[11:8] is reserved.
     * - tag_id[15:12] contains the depth for this chunk from 0 to 15.
     *   - 0 = block (sample level)
     *   - 1 = First-level summary of block samples
     *   - 2 = Second-level summary of first-level summaries.
     */
    uint16_t chunk_meta;
    
    /**
     * @brief The length of the payload in bytes.  Can be 0.
     *
     * In addition to defining the payload size, this value is
     * also used for forward chunk traversal.
     */
    uint32_t payload_length;
    
    /**
     * @brief The length of the previous payload in bytes.
     *
     * Used for reverse chunk traversal.
     */
    uint32_t payload_prev_length;
    
    /// The CRC32 over the header, excluding this field.
    uint32_t crc32;
};


/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* JLS_FORMAT_H__ */
