#include "source_file.h"
#include "utils.h"
#include "log.h"

static char * position_to_offset(const source_file_t * p_file, const position_t * p_pos)
{
    char * p_offset = p_file->p_contents;
    for (unsigned i = 0; i < p_pos->line && p_offset && p_offset < p_file->p_contents + p_file->size; ++i, p_offset++)
    {
        LOG("Looking for line ending #%u -> %p\n", i, p_offset);
        p_offset = strchr(p_offset, '\n');
    }
    LOG("OFFSET: %p\n", p_offset);
    return p_offset + p_pos->character;
}

source_file_t * source_file_create(const char * p_contents)
{
    source_file_t * p_source_file = MALLOC(sizeof(source_file_t));
    ASSERT(p_source_file);
    p_source_file->p_contents = STRDUP(p_contents);
    p_source_file->size = strlen(p_contents);
    return p_source_file;
}

void source_file_contents_set(source_file_t * p_source_file, const char * p_new_contents)
{
    ASSERT(p_source_file->p_contents);
    ASSERT(p_source_file->p_contents);
    FREE(p_source_file->p_contents);
    p_source_file->p_contents = STRDUP(p_new_contents);
    p_source_file->size = strlen(p_new_contents);
}

void source_file_patch(source_file_t * p_source_file, const char * p_new_contents, const position_t * p_start_pos, size_t old_len)
{
    LOG("Patching file\n");
    size_t new_len = strlen(p_new_contents);
    char * p_start_offset = position_to_offset(p_source_file, p_start_pos);
    ASSERT(p_start_offset);
    LOG("LINE %u => OFFSET %u\n", p_start_pos->line, &p_start_offset[0] - &p_source_file->p_contents[0]);

    if (new_len == old_len)
    {
        memcpy(p_start_offset, p_new_contents, new_len);
    }
    else if (new_len < old_len)
    {
        size_t remainder = p_source_file->size - (p_start_offset + old_len - p_source_file->p_contents) + 1;
        memcpy(p_start_offset, p_new_contents, new_len);
        memmove(p_start_offset + new_len, p_start_offset + old_len, remainder);
        p_source_file->size -= (old_len - new_len);
        p_source_file->p_contents = REALLOC(p_source_file->p_contents, p_source_file->size + 1);
    }
    else
    {
        size_t remainder = p_source_file->size - (p_start_offset + old_len - p_source_file->p_contents) + 1;
        p_source_file->size += (new_len - old_len);
        p_source_file->p_contents = REALLOC(p_source_file->p_contents, p_source_file->size + 1);
        memmove(p_start_offset + new_len, p_start_offset + old_len, remainder);
        memcpy(p_start_offset, p_new_contents, new_len);
    }
    LOG("File contents:%s\n", p_source_file->p_contents);
}

void source_file_free(source_file_t * p_source_file)
{
    ASSERT(p_source_file);
    ASSERT(p_source_file->p_contents);
    FREE(p_source_file->p_contents);
    FREE(p_source_file);

}