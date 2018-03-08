#include <stdbool.h>
#include <string.h>
#include "command_handler.h"


#define DECLARE_JSON_CHILD(ROOT, CHILD) \
    json_t * p_##CHILD = json_object_get(ROOT, #CHILD);




static const char * mp_command_type_strings[] = {
    "set_file",
    "complete_at",
    "function_definition_get",
    "definition_get",
    "references_get",
    "exit"
};

static bool unsaved_file_parse(json_t * p_json, unsaved_file_t * p_unsaved_file)
{
    DECLARE_JSON_CHILD(p_json, name);
    DECLARE_JSON_CHILD(p_json, contents);
    if (!json_is_string(p_name) || !json_is_string(p_contents))
    {
        json_decref(p_name);
        json_decref(p_contents);
        return false;
    }

    p_unsaved_file->Filename = STRDUP(json_string_value(p_name));
    p_unsaved_file->Contents = STRDUP(json_string_value(p_contents));
    p_unsaved_file->Length = strlen(p_unsaved_file->Contents);

    json_decref(p_name);
    json_decref(p_contents);
    return true;
}

static void command_params_set_file_t_free(command_params_set_file_t * p_params)
{
    FREE((void *) p_params->p_filename);

    for (unsigned i = 0; i < p_params->define_count; ++i)
    {
        FREE((void *) p_params->pp_defines[i]);
    }
    FREE((void *) p_params->pp_defines);

    for (unsigned i = 0; i < p_params->include_count; ++i)
    {
        FREE((void *) p_params->pp_includes[i]);
    }
    FREE((void *) p_params->pp_includes);

    for (unsigned i = 0; i < p_params->unsaved_file_count; ++i)
    {
        FREE((void *) p_params->p_unsaved_files[i].Filename);
        FREE((void *) p_params->p_unsaved_files[i].Contents);
    }
    FREE(p_params->p_unsaved_files);
}

static bool command_params_set_file_t_parse(json_t * p_json, command_params_set_file_t * p_params)
{
    DECLARE_JSON_CHILD(p_json, filename);
    DECLARE_JSON_CHILD(p_json, defines);
    DECLARE_JSON_CHILD(p_json, includes);
    DECLARE_JSON_CHILD(p_json, unsaved_files);

    if (!json_is_string(p_filename) || !json_is_array(p_defines) || !json_is_array(p_includes) || (p_unsaved_files != NULL && !json_is_array(p_unsaved_files)))
    {
        json_decref(p_filename);
        json_decref(p_defines);
        json_decref(p_includes);
        json_decref(p_unsaved_files);
        return false;
    }

    p_params->p_filename = STRDUP(json_string_value(p_filename));

    p_params->define_count = json_array_size(p_defines);
    p_params->include_count = json_array_size(p_includes);
    p_params->unsaved_file_count = json_array_size(p_unsaved_files);

    p_params->pp_defines      = CALLOC(p_params->define_count, sizeof(p_params->pp_defines[0]));
    p_params->pp_includes     = CALLOC(p_params->include_count, sizeof(p_params->pp_includes[0]));
    p_params->p_unsaved_files = CALLOC(p_params->unsaved_file_count, sizeof(p_params->p_unsaved_files[0]));

    unsigned i;
    json_t * p_define;
    json_array_foreach(p_defines, i, p_define)
    {
        p_params->pp_defines[i] = STRDUP(json_string_value(p_define));
        json_decref(p_define);
    }
    json_t * p_include;
    json_array_foreach(p_includes, i, p_include)
    {
        p_params->pp_includes[i] = STRDUP(json_string_value(p_include));
        json_decref(p_include);
    }

    bool valid_params = true;

    if (p_unsaved_files != NULL)
    {
        json_t * p_unsaved_file;
        json_array_foreach(p_unsaved_files, i, p_unsaved_file)
        {
            if (!unsaved_file_parse(p_unsaved_file, &p_params->p_unsaved_files[i]))
            {
                command_params_set_file_t_free(p_params);
                valid_params = false;
                break;
            }
            json_decref(p_unsaved_file);
        }
    }

    json_decref(p_filename);
    json_decref(p_defines);
    json_decref(p_includes);
    json_decref(p_unsaved_files);

    return valid_params;
}

static void command_params_getter_t_free(command_params_getter_t * p_params)
{
    for (unsigned i = 0; i < p_params->unsaved_file_count; ++i)
    {
        FREE((void *) p_params->p_unsaved_files[i].Filename);
        FREE((void *) p_params->p_unsaved_files[i].Contents);
    }
    FREE(p_params->p_unsaved_files);
}

static bool command_params_getter_t_parse(json_t * p_json, command_params_getter_t * p_params)
{
    DECLARE_JSON_CHILD(p_json, line);
    DECLARE_JSON_CHILD(p_json, column);
    DECLARE_JSON_CHILD(p_json, unsaved_files);

    p_params->line = (unsigned) json_integer_value(p_line);
    p_params->column = (unsigned) json_integer_value(p_column);

    p_params->unsaved_file_count = json_array_size(p_unsaved_files);
    p_params->p_unsaved_files = CALLOC(p_params->unsaved_file_count, sizeof(p_params->p_unsaved_files[0]));

    unsigned i;
    json_t * p_unsaved_file;
    json_array_foreach(p_unsaved_files, i, p_unsaved_file)
    {
        if (!unsaved_file_parse(p_unsaved_file, &p_params->p_unsaved_files[i]))
        {
            command_params_getter_t_free(p_params);
            return false;
        }
        json_decref(p_unsaved_file);
    }

    return true;
}

const command_t * command_parse(const char * p_method, json_t * p_params)
{
    command_t * p_command = MALLOC(sizeof(command_t));
    p_command->type = command_type_get(p_method);
    bool valid_params;
    switch (p_command->type)
    {
        case COMMAND_TYPE_SET_FILE:
            valid_params = command_params_set_file_t_parse(p_params, &p_command->params.set_file);
            break;
        case COMMAND_TYPE_COMPLETE_AT:
        case COMMAND_TYPE_FUNCTION_DEFINITION_GET:
        case COMMAND_TYPE_DEFINITION_GET:
        case COMMAND_TYPE_REFERENCES_GET:
            valid_params = command_params_getter_t_parse(p_params, &p_command->params.definition_get);
            break;
        case COMMAND_TYPE_EXIT:
            valid_params = (p_params == NULL || json_is_null(p_params));
            break;
        default:
            valid_params = false;
    }

    if (valid_params)
    {
        return p_command;
    }
    else
    {
        FREE(p_command);
        return NULL;
    }
}


void command_free(command_t * p_command)
{
    switch (p_command->type)
    {
        case COMMAND_TYPE_SET_FILE:
             command_params_set_file_t_free(&p_command->params.set_file);
             break;
        case COMMAND_TYPE_COMPLETE_AT:
        case COMMAND_TYPE_FUNCTION_DEFINITION_GET:
        case COMMAND_TYPE_DEFINITION_GET:
        case COMMAND_TYPE_REFERENCES_GET:
            command_params_getter_t_free(&p_command->params.complete_at);
            break;
    }
    FREE(p_command);
}

command_type_t command_type_get(const char * p_method)
{
    if (p_method != NULL)
    {
        for (unsigned i = 0; i < COMMAND_TYPE_COUNT; ++i)
        {
            if (strcmp(p_method, mp_command_type_strings[i]) == 0)
            {
                return (command_type_t) i;
            }
        }
    }
    return COMMAND_TYPE_UNKNOWN;
}

const char * command_name_get(command_type_t type)
{
    if (type < COMMAND_TYPE_COUNT)
    {
        return mp_command_type_strings[type];
    }
    else
    {
        return NULL;
    }
}
