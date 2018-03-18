#pragma once
#include <stdbool.h>

typedef struct
{
    const char * scheme;
    const char * authority;
    const char * path;
    const char * query;
    const char * fragment;
} uri_t;

/**
 * Parse the given string as an URI. Allocates the URI parameters on the heap, without taking references
 * to the raw representation.
 */
uri_t uri_decode(const char * p_raw);

char * uri_encode(uri_t * p_uri);

void uri_free_members(uri_t * p_uri);

uri_t uri_file(const char * p_filename);

char * uri_file_encode(const char * p_filename);

const char * uri_file_extension(const uri_t * p_uri);

const bool uri_is_c_cpp_file(const uri_t * p_uri);