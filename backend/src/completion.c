#include "completion.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <clang-c/Index.h>

#define TRANSLATION_UNIT_FLAGS (CXTranslationUnit_IncludeBriefCommentsInCodeCompletion | CXTranslationUnit_KeepGoing | CXTranslationUnit_CacheCompletionResults)
#define COMPLETION_FLAGS (CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeBriefComments | CXCodeComplete_IncludeCodePatterns)

#define COMPLETION_STRING_MAXLEN    2048

typedef struct
{
    completion_reference_callback_t user_callback;
    void * p_user_args;
    unsigned current_index;
} reference_visitor_context_t;


static const char * mp_base_flags[] = {
    "-Wdocumentation",
    "-Wall"
};

static CXTranslationUnit m_translation_unit;
static CXIndex m_index;
static bool m_active;
static char * mp_filename;

static completion_kind_t get_kind(enum CXCursorKind kind)
{
    switch (kind)
    {
        case CXCursor_StructDecl:
            return COMPLETION_KIND_STRUCT;

        case CXCursor_UnionDecl:
            return COMPLETION_KIND_UNION;

        case CXCursor_ClassDecl:
            return COMPLETION_KIND_CLASS;

        case CXCursor_EnumConstantDecl:
            return COMPLETION_KIND_ENUM_CONSTANT;

        case CXCursor_EnumDecl:
            return COMPLETION_KIND_ENUM;

        case CXCursor_MacroDefinition:
        case CXCursor_MacroExpansion:
            return COMPLETION_KIND_MACRO;

        case CXCursor_FieldDecl:
            return COMPLETION_KIND_FIELD;

        case CXCursor_FunctionDecl:
        case CXCursor_Constructor:
        case CXCursor_Destructor:
        case CXCursor_ConversionFunction:
        case CXCursor_CXXMethod:
            return COMPLETION_KIND_FUNCTION;

        case CXCursor_VarDecl:
        case CXCursor_IntegerLiteral:
        case CXCursor_FloatingLiteral:
        case CXCursor_ImaginaryLiteral:
        case CXCursor_StringLiteral:
        case CXCursor_CharacterLiteral:
            return COMPLETION_KIND_VARIABLE;

        case CXCursor_ParmDecl:
            return COMPLETION_KIND_PARAMETER;

        case CXCursor_Namespace:
            return COMPLETION_KIND_NAMESPACE;

        case CXCursor_TypedefDecl:
            return COMPLETION_KIND_TYPEDEF;
        case CXCursor_NotImplemented:
            return COMPLETION_KIND_LANGUAGE_TOKEN;
        default:
            return COMPLETION_KIND_UNKNOWN;
    }
}

static CXCursor cursor_get(unsigned line, unsigned column)
{
    CXSourceLocation location = clang_getLocation(m_translation_unit, clang_getFile(m_translation_unit, mp_filename), line, column);
    return clang_getCursor(m_translation_unit, location);
}

static CXCursor definition_cursor_get(unsigned line, unsigned column)
{
    CXCursor cursor = cursor_get(line, column);
    return clang_getCursorDefinition(cursor);
}

static const completion_definition_t * definition_get(CXCursor definition_cursor)
{
    CXType type = clang_getCursorType(definition_cursor);

    CXString name = clang_getCursorSpelling(definition_cursor);
    CXString type_name = clang_getTypeSpelling(type);
    CXString documentation = clang_Cursor_getRawCommentText(definition_cursor);
    CXSourceLocation definition_location = clang_getCursorLocation(definition_cursor);

    CXFile file;
    unsigned definition_line;
    unsigned definition_column;
    clang_getSpellingLocation(definition_location,
                                &file,
                                &definition_line,
                                &definition_column,
                                NULL);

    completion_definition_t * p_definition = MALLOC(sizeof(completion_definition_t));
    p_definition->p_name = STRDUP(clang_getCString(name));
    p_definition->p_type = STRDUP(clang_getCString(type_name));
    p_definition->p_documentation = STRDUP(clang_getCString(documentation));
    p_definition->location.p_filename = STRDUP(clang_getCString(clang_getFileName(file)));
    p_definition->location.line = definition_line;
    p_definition->location.column = definition_column;
    p_definition->kind = get_kind(clang_getCursorKind(definition_cursor));

    clang_disposeString(name);
    clang_disposeString(type_name);
    clang_disposeString(documentation);

    return p_definition;
}

static bool reparse_translation_unit(unsaved_file_t * p_unsaved_files, unsigned unsaved_file_count)
{
    if (unsaved_file_count > 0)
    {
        if (clang_reparseTranslationUnit(m_translation_unit,
                                         unsaved_file_count,
                                         p_unsaved_files,
                                         clang_defaultReparseOptions(m_translation_unit)) != CXError_Success)
        {
            return false;
        }
    }
    return true;
}

static void definition_members_free(const completion_definition_t * p_definition)
{
    FREE((void *) p_definition->p_name);
    FREE((void *) p_definition->p_type);
    FREE((void *) p_definition->p_documentation);
    FREE((void *) p_definition->location.p_filename);
}


static enum CXVisitorResult reference_visitor_callback(void * p_context, CXCursor cursor, CXSourceRange range)
{
    reference_visitor_context_t * p_visitor_context = p_context;
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    completion_location_t reference_location;

    clang_getFileLocation(location, &file, &reference_location.line, &reference_location.column, NULL);
    CXString filename = clang_getFileName(file);
    reference_location.p_filename = clang_getCString(filename);

    completion_reference_kind_t reference_kind;
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind >= CXCursor_FirstDecl && kind <= CXCursor_LastDecl)
    {
        reference_kind = COMPLETION_REFERENCE_KIND_DEFINITION;
    }
    else
    {
        reference_kind = COMPLETION_REFERENCE_KIND_REFERENCE;
    }

    p_visitor_context->user_callback(reference_kind, &reference_location, p_visitor_context->current_index++, p_visitor_context->p_user_args);

    clang_disposeString(filename);
    return CXVisit_Continue;
}

void completion_init(void)
{
    m_index = clang_createIndex(0, 1);
    m_active = false;
}


bool completion_set_file(const char * p_filename,
                         const char * const * p_defines,
                         unsigned define_count,
                         const char * const * p_includes,
                         unsigned include_count,
                         unsaved_file_t * p_unsaved_files,
                         unsigned unsaved_file_count)
{
    if (m_active)
    {
        FREE(mp_filename);
        clang_disposeTranslationUnit(m_translation_unit);
    }

    char ** pp_flags = NULL;
    unsigned flag_count = define_count + include_count;

    if (flag_count > 0)
    {
        pp_flags = MALLOC(sizeof(char *) * flag_count);
        unsigned flag_index = 0;

        const struct
        {
            const char * p_prefix;
            const char * const * p_items;
            unsigned item_count;
        } flag_list[] = {
            {
                "",
                mp_base_flags,
                sizeof(mp_base_flags) / sizeof(mp_base_flags[0])
            },
            {
                "-D",
                p_defines,
                define_count
            },
            {
                "-I",
                p_includes,
                include_count
            }
        };

        for (unsigned i = 0; i < sizeof(flag_list) / sizeof(flag_list[0]); ++i)
        {
            unsigned prefix_len = strlen(flag_list[i].p_prefix);

            for (unsigned j = 0; j < flag_list[i].item_count; ++j)
            {
                char ** pp_flag = &pp_flags[flag_index++];
                *pp_flag = MALLOC(prefix_len + strlen(flag_list[i].p_items[j]) + 1);
                if (prefix_len > 0)
                {
                    memcpy(*pp_flag, flag_list[i].p_prefix, prefix_len);
                }
                strcpy(&(*pp_flag)[prefix_len], flag_list[i].p_items[j]);
            }
        }
    }

    mp_filename = STRDUP(p_filename);

    enum CXErrorCode error = clang_parseTranslationUnit2(m_index,
                                                         p_filename,
                                                         pp_flags, flag_count,
                                                         p_unsaved_files, unsaved_file_count,
                                                         TRANSLATION_UNIT_FLAGS,
                                                         &m_translation_unit);

    m_active = (error == CXError_Success);

    if (pp_flags != NULL)
    {
        for (unsigned i = 0; i < flag_count; ++i)
        {
            FREE(pp_flags[i]);
        }
        FREE(pp_flags);
    }
    return m_active;
}

const unsigned completion_get(unsigned line,
                              unsigned column,
                              unsaved_file_t * p_unsaved_files,
                              unsigned unsaved_file_count,
                              completion_result_callback_t callback,
                              void * p_args)
{
    if (!m_active || callback == NULL)
    {
        return 0;
    }

    CXCodeCompleteResults * p_results = clang_codeCompleteAt(m_translation_unit,
                                                             mp_filename,
                                                             line,
                                                             column, NULL, 0, COMPLETION_FLAGS);
    unsigned result_count = p_results->NumResults;
    if (p_results)
    {
        for (unsigned i = 0; i < result_count; ++i)
        {
            CXCompletionString completion = p_results->Results[i].CompletionString;
            CXString doc = clang_getCompletionBriefComment(completion);
            CXString typed_text;
            CXString inserted_text;
            bool has_inserted_text = false;
            CXString var_type_text;
            bool has_var_type_text = false;
            char * p_full_text = MALLOC(COMPLETION_STRING_MAXLEN);
            char * p_full_text_next = p_full_text;
            unsigned chunk_count = clang_getNumCompletionChunks(completion);

            for (unsigned j = 0; j < chunk_count; ++j)
            {
                enum CXCompletionChunkKind kind = clang_getCompletionChunkKind(completion, j);
                CXString chunk_string           = clang_getCompletionChunkText(completion, j);

                const char * p_text = clang_getCString(chunk_string);
                unsigned text_len = strlen(p_text);

                if (kind != CXCompletionChunk_ResultType)
                {
                    if (p_full_text_next + text_len < p_full_text + COMPLETION_STRING_MAXLEN)
                    {
                        strcpy(p_full_text_next, p_text);
                        p_full_text_next += text_len;
                    }
                }

                switch (kind)
                {
                    case CXCompletionChunk_TypedText:
                        typed_text = chunk_string;
                        break;

                    case CXCompletionChunk_Text:
                        inserted_text = chunk_string;
                        has_inserted_text = true;
                        break;

                    case CXCompletionChunk_ResultType:
                        var_type_text = chunk_string;
                        has_var_type_text = true;
                        break;

                    default:
                        clang_disposeString(chunk_string);
                        break;
                }
            }

            completion_result_t result;
            result.index = i;
            result.p_documentation = clang_getCString(doc);
            result.p_typed_text = clang_getCString(typed_text);
            result.p_inserted_text = has_inserted_text ? clang_getCString(inserted_text) : result.p_typed_text;
            result.p_display_value = p_full_text;
            result.p_type = has_var_type_text ? clang_getCString(var_type_text) : NULL;
            result.kind = get_kind(p_results->Results[i].CursorKind);
            result.priority = clang_getCompletionPriority(completion);
            result.availability = clang_getCompletionAvailability(completion);

            callback(&result, result_count, p_args);

            FREE(p_full_text);
            clang_disposeString(doc);
            clang_disposeString(typed_text);


            if (has_inserted_text)
            {
                clang_disposeString(inserted_text);
            }
            if (has_var_type_text)
            {
                clang_disposeString(var_type_text);
            }
        }

        clang_disposeCodeCompleteResults(p_results);
        return result_count;
    }
    else
    {
        return 0;
    }
}

const completion_function_definition_t * completion_function_definition_get(unsigned line,
                                                            unsigned column,
                                                            unsaved_file_t * p_unsaved_files,
                                                            unsigned unsaved_file_count)
{
    if (!reparse_translation_unit(p_unsaved_files, unsaved_file_count))
    {
        return NULL;
    }

    CXCursor definition_cursor = definition_cursor_get(line, column);

    int arg_count = clang_Cursor_getNumArguments(definition_cursor);

    completion_function_definition_t * p_retval = NULL;

    if (arg_count >= 0) {
        p_retval = MALLOC(sizeof(completion_function_definition_t));

        const completion_definition_t * p_definition = definition_get(definition_cursor);
        p_retval->definition = *p_definition;
        /* Free definition structure, but not its members: */
        FREE((void *) p_definition);

        CXType return_type = clang_getCursorResultType(definition_cursor);
        CXString return_type_name = clang_getTypeSpelling(return_type);

        p_retval->p_return_type = STRDUP(clang_getCString(return_type_name));
        p_retval->param_count = arg_count;

        completion_fuction_param_t * p_params = NULL;
        if (arg_count > 0)
        {
            p_params = MALLOC(sizeof(completion_fuction_param_t) * arg_count);
        }

        for (unsigned i = 0; i < (unsigned) arg_count; ++i)
        {
            CXCursor arg = clang_Cursor_getArgument(definition_cursor, i);
            CXType type = clang_getCursorType(arg);

            CXString type_name = clang_getTypeSpelling(type);
            CXString name = clang_getCursorSpelling(arg);

            p_params[i].p_name = STRDUP(clang_getCString(name));
            p_params[i].p_type = STRDUP(clang_getCString(type_name));
            p_params[i].p_default_value = NULL;

            clang_disposeString(type_name);
            clang_disposeString(name);
        }
        p_retval->p_params = p_params;

        clang_disposeString(return_type_name);
    }

    return p_retval;
}

void completion_function_definition_free(const completion_function_definition_t * p_function_definition)
{
    if (p_function_definition != NULL)
    {
        for (unsigned i = 0; i < p_function_definition->param_count; ++i)
        {
            FREE((void *) p_function_definition->p_params[i].p_name);
            FREE((void *) p_function_definition->p_params[i].p_type);
            FREE((void *) p_function_definition->p_params[i].p_default_value);
        }

        FREE((void *) p_function_definition->p_params);
        FREE((void *) p_function_definition->p_return_type);
        definition_members_free(&p_function_definition->definition);
        FREE((void *) p_function_definition);
    }
}


const completion_definition_t * completion_definition_get(unsigned line,
                                                          unsigned column,
                                                          unsaved_file_t * p_unsaved_files,
                                                          unsigned unsaved_file_count)
{
    if (!reparse_translation_unit(p_unsaved_files, unsaved_file_count))
    {
        return NULL;
    }

    CXCursor definition_cursor = definition_cursor_get(line, column);
    if (clang_Cursor_isNull(definition_cursor))
    {
        return NULL;
    }
    return definition_get(definition_cursor);
}

void completion_definition_free(const completion_definition_t * p_definition)
{
    definition_members_free(p_definition);
    FREE((void *) p_definition);
}

unsigned completion_references_get(unsigned line,
                                   unsigned column,
                                   unsaved_file_t * p_unsaved_files,
                                   unsigned unsaved_file_count,
                                   completion_reference_callback_t callback,
                                   void *p_args)
{
    if (!reparse_translation_unit(p_unsaved_files, unsaved_file_count))
    {
        return 0;
    }
    CXFile file = clang_getFile(m_translation_unit, mp_filename);
    CXSourceLocation location = clang_getLocation(m_translation_unit, file, line, column);
    CXCursor cursor = clang_getCursor(m_translation_unit, location);

    reference_visitor_context_t context;
    context.p_user_args = p_args;
    context.user_callback = callback;
    context.current_index = 0;
    CXCursorAndRangeVisitor visitor;
    visitor.context = &context;
    visitor.visit = reference_visitor_callback;
    if (clang_findReferencesInFile(cursor, file, visitor) != CXResult_Success)
    {
        return 0;
    }

    return context.current_index;
}