#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "completion.h"

static const char * mp_kind[] = {
    "FIELD",
    "STRUCT",
    "UNION",
    "CLASS",
    "ENUM",
    "ENUM_CONSTANT",
    "MACRO",
    "FUNCTION",
    "VARIABLE",
    "PARAMETER",
    "NAMESPACE",
    "TYPEDEF",
    "LANGUAGE_TOKEN",
    "UNKNOWN"
};

static const char * mp_availability[] = {
    "AVAILABLE",
    "DEPRECATED",
    "NOT_AVAILABLE",
    "NOT_ACCESSIBLE"
};


static void print_usage(void)
{
    printf("USAGE: backend.exe <filename> <line> <column>\n");
}

static void completion_result(const completion_result_t * p_result, unsigned total, void * p_args)
{
    printf("RESULT %u/%u:\n", p_result->index + 1, total);
    printf("\tDOCUMENTATION:    %s\n", p_result->p_documentation);
    printf("\tTYPED_TEXT:       %s\n", p_result->p_typed_text);
    printf("\tINSERTED_TEXT:    %s\n", p_result->p_inserted_text);
    printf("\tDISPLAY_VALUE:    %s\n", p_result->p_display_value);
    printf("\tTYPE:             %s\n", p_result->p_type);
    printf("\tKIND:             %s\n", mp_kind[p_result->kind]);
    printf("\tAVAILABILITY:     %s\n", mp_availability[p_result->availability]);
    printf("\tPRIORITY:         %u\n", p_result->priority);
}

static void reference_result(completion_reference_kind_t kind, const completion_location_t * p_location, unsigned index, void * p_args)
{
    printf("LOCATION:       %s:%u:%u (%s)\n",
           p_location->p_filename,
           p_location->line,
           p_location->column,
           (kind == COMPLETION_REFERENCE_KIND_REFERENCE ? "Reference" : "Definition"));
}

int main(unsigned args, const char ** pp_argv)
{
    if (args != 4)
    {
        printf("Got %u arguments, expected 4.\n", args);
        print_usage();
        return 1;
    }

    completion_init();
    completion_set_file(pp_argv[1], NULL, 0, NULL, 0, NULL, 0);

#if 0
    unsigned result_count = completion_get(atoi(pp_argv[2]),
                                           atoi(pp_argv[3]),
                                           NULL, 0,
                                           completion_result,
                                           NULL);

    printf("----------------------------------------\nGot %u results.\n", result_count);
#elif 0
    const completion_function_definition_t *p_params = completion_function_definition_get(atoi(pp_argv[2]),
                                                                          atoi(pp_argv[3]),
                                                                          NULL, 0);
    if (p_params != NULL)
    {
        printf("FUNCTION:       %s\n", p_params->definition.p_name);
        printf("RETURN TYPE:    %s\n", p_params->definition.p_type);
        printf("DOCUMENTATION:  %s\n", p_params->definition.p_documentation);
        printf("KIND:           %s\n", mp_kind[p_params->definition.kind]);
        printf("LOCATION:       %s:%u:%u\n",
               p_params->definition.location.p_filename,
               p_params->definition.location.line,
               p_params->definition.location.column);
        for (unsigned i = 0; i < p_params->param_count; ++i)
        {
            printf("PARAM %u/%u\n", i + 1, p_params->param_count);
            printf("\tNAME:          %s\n", p_params->p_params[i].p_name);
            printf("\tTYPE:          %s\n", p_params->p_params[i].p_type);
        }
        completion_function_definition_free(p_params);
    }
    else
    {
        printf("No function at %s: %s\n", pp_argv[2], pp_argv[3]);
    }
#elif 0
    const completion_definition_t *p_definition = completion_definition_get(atoi(pp_argv[2]), atoi(pp_argv[3]), NULL, 0);

    if (p_definition != NULL)
    {
        printf("NAME:           %s\n", p_definition->p_name);
        printf("TYPE:           %s\n", p_definition->p_type);
        printf("DOCUMENTATION:  %s\n", p_definition->p_documentation);
        printf("KIND:           %s (%u)\n", mp_kind[p_definition->kind], p_definition->kind);
        printf("LOCATION:       %s:%u:%u\n",
               p_definition->location.p_filename,
               p_definition->location.line,
               p_definition->location.column);

        completion_definition_free(p_definition);
    }
    else
    {
        printf("No symbol at %s: %s\n", pp_argv[2], pp_argv[3]);
    }
#else
    printf("REFERENCES: %u\n", completion_references_get(atoi(pp_argv[2]), atoi(pp_argv[3]), NULL, 0, reference_result, NULL));
#endif
    return 0;
}