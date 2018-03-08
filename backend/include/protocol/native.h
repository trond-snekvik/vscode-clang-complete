#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <jansson.h>
#include <string.h>
#include "uri.h"
#include "assert.h"
#include "utils.h"
/**
 * @file Parsing of native types from json
 */


static inline uri_t decode_uri(json_t * p_json)
{
#ifdef PROTOCOL_DEBUG
    ASSERT(json_is_string(p_json));
#endif
    return uri_decode(json_string_value(p_json));
}

static inline char * decode_string(json_t * p_json)
{
#ifdef PROTOCOL_DEBUG
    ASSERT(json_is_string(p_json));
#endif
    return STRDUP(json_string_value(p_json));
}

static inline int64_t decode_number(json_t * p_json)
{
#ifdef PROTOCOL_DEBUG
    ASSERT(json_is_integer(p_json));
#endif
    return json_integer_value(p_json);
}

static inline bool decode_boolean(json_t * p_json)
{
#ifdef PROTOCOL_DEBUG
    ASSERT(json_is_boolean(p_json));
#endif
    return json_boolean_value(p_json);
}

static inline json_t * encode_uri(uri_t value)
{
    char * p_encoded = uri_encode(&value);
    json_t * p_retval = json_string(p_encoded);
    free(p_encoded);
    return p_retval;
}

static inline json_t * encode_string(char * value)
{
    return json_string(value);
}

static inline json_t * encode_number(int64_t value)
{
    return json_integer(value);
}

static inline json_t * encode_boolean(bool value)
{
    return json_boolean(value);
}

static inline void free_uri(uri_t uri)
{
    uri_free_members(&uri);
}

static inline void free_string(char * string)
{
    free(string);
}

static inline void free_number(int64_t num) {}
static inline void free_boolean(bool b) {}