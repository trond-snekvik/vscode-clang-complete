#include <stdlib.h>
#include "unit.h"
#include "log.h"
#include "utils.h"
#include "path.h"
#include "doxygen.h"
#include "decoders.h"
#include "encoders.h"

#define TRANSLATION_UNIT_PARSE_OPTIONS (CXTranslationUnit_PrecompiledPreamble |                  \
                                        CXTranslationUnit_CacheCompletionResults |               \
                                        CXTranslationUnit_IncludeBriefCommentsInCodeCompletion | \
                                        CXTranslationUnit_CreatePreambleOnFirstParse |           \
                                        CXTranslationUnit_KeepGoing)

#define TRANSLATION_UNIT_REPARSE_OPTIONS (CXReparse_None)

#define COMPLETION_FLAGS (CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeBriefComments)
#define COMPLETION_STRING_MAXLEN    2048

#define DIAG_MAX_PER_FILE 100
#define DIAG_MAX_FILES 20


typedef struct
{
    char * p_name;
    char * p_type;
    char * p_documentation;
    text_document_position_params_t location;
    completion_item_kind_t kind;
} definition_t;

typedef struct
{
    bool found_file;
    char * p_filename;
} inclusion_file_context_t;

static CXIndex m_index;

static unit_config_t m_config;
static unit_diagnostics_callback_t m_diagnostics_callback;

static bool position_equal(const position_t * p_pos1, const position_t * p_pos2)
{
    return p_pos1->line == p_pos2->line && p_pos1->character == p_pos2->character;
}

static bool range_is_zero_length(const range_t * p_range)
{
    return position_equal(&p_range->start, &p_range->end);
}

static bool range_contains(const range_t * p_range, const position_t * p_pos)
{
    return ((p_pos->line > p_range->start.line || (p_pos->line == p_range->start.line && p_pos->character >= p_range->start.character)) &&
            (p_pos->line < p_range->end.line   || (p_pos->line == p_range->end.line   && p_pos->character <= p_range->end.character)));
}

static bool range_overlap(const range_t * p_range1, const range_t * p_range2)
{
    return (range_contains(p_range1, &p_range2->start) || range_contains(p_range1, &p_range2->end) ||
            range_contains(p_range2, &p_range1->start) || range_contains(p_range2, &p_range1->end));
}

static completion_item_kind_t get_kind(enum CXCursorKind kind)
{
    switch (kind)
    {
        case CXCursor_StructDecl:
            return COMPLETION_ITEM_KIND_INTERFACE;

        case CXCursor_UnionDecl:
            return COMPLETION_ITEM_KIND_INTERFACE;

        case CXCursor_ClassDecl:
            return COMPLETION_ITEM_KIND_CLASS;

        case CXCursor_EnumConstantDecl:
        case CXCursor_EnumDecl:
            return COMPLETION_ITEM_KIND_ENUM;

        case CXCursor_MacroDefinition:
        case CXCursor_MacroExpansion:
            return COMPLETION_ITEM_KIND_FUNCTION;

        case CXCursor_FieldDecl:
            return COMPLETION_ITEM_KIND_FIELD;

        case CXCursor_Constructor:
            return COMPLETION_ITEM_KIND_CONSTRUCTOR;
        case CXCursor_Destructor:
        case CXCursor_FunctionDecl:
        case CXCursor_ConversionFunction:
        case CXCursor_CXXMethod:
            return COMPLETION_ITEM_KIND_FUNCTION;

        case CXCursor_VarDecl:
        case CXCursor_IntegerLiteral:
        case CXCursor_FloatingLiteral:
        case CXCursor_ImaginaryLiteral:
        case CXCursor_StringLiteral:
        case CXCursor_CharacterLiteral:
            return COMPLETION_ITEM_KIND_VARIABLE;

        case CXCursor_InclusionDirective:
            return COMPLETION_ITEM_KIND_FILE;

        case CXCursor_ParmDecl:
            return COMPLETION_ITEM_KIND_VARIABLE;

        case CXCursor_Namespace:
            return COMPLETION_ITEM_KIND_UNIT;

        case CXCursor_TypedefDecl:
        case CXCursor_NotImplemented:
            return COMPLETION_ITEM_KIND_KEYWORD;
        default:
            return COMPLETION_ITEM_KIND_TEXT;
    }
}

static diagnostic_severity_t get_diagnostic_severity(enum CXDiagnosticSeverity s)
{
    switch (s)
    {
        case CXDiagnostic_Ignored:
        case CXDiagnostic_Note:
            return DIAGNOSTIC_SEVERITY_INFORMATION;
        case CXDiagnostic_Warning:
            return DIAGNOSTIC_SEVERITY_WARNING;
        case CXDiagnostic_Error:
        case CXDiagnostic_Fatal:
            return DIAGNOSTIC_SEVERITY_ERROR;
        default:
            ASSERT(false);
    }
    return DIAGNOSTIC_SEVERITY_HINT;
}

static bool diagnostic_parse(CXDiagnostic clang_diag, diagnostic_t * p_diagnostic, CXFile * p_file)
{
    enum CXDiagnosticSeverity s = clang_getDiagnosticSeverity(clang_diag);
    if (s == CXDiagnostic_Ignored)
    {
        /* Don't emit ignored diagnostics */
        return false;
    }
    CXString diagstring = clang_getDiagnosticSpelling(clang_diag);
    CXSourceLocation location = clang_getDiagnosticLocation(clang_diag);
    unsigned line;
    unsigned character;
    clang_getSpellingLocation(location,
                              p_file,
                              &line,
                              &character,
                              NULL);
    p_diagnostic->range.start.line = (line > 0 ? line - 1 : 0); // with invalid files, line will be 0
    p_diagnostic->range.start.character = character;
    p_diagnostic->range.start.valid_fields = POSITION_FIELD_ALL;
    p_diagnostic->range.end = p_diagnostic->range.start;
    p_diagnostic->range.valid_fields = RANGE_FIELD_ALL;

    p_diagnostic->severity = get_diagnostic_severity(s);
    p_diagnostic->code = clang_getDiagnosticCategory(clang_diag);
    p_diagnostic->source = "Clang";
    p_diagnostic->message = STRDUP(clang_getCString(diagstring));
    p_diagnostic->valid_fields = DIAGNOSTIC_FIELD_ALL;
    clang_disposeString(diagstring);
    return true;
}

static CXCursor cursor_get(unit_t * p_unit, const char * p_filename, unsigned line, unsigned column)
{
    CXSourceLocation location = clang_getLocation(p_unit->tu, clang_getFile(p_unit->tu, p_filename), line, column);
    return clang_getCursor(p_unit->tu, location);
}

static CXCursor definition_cursor_get(CXCursor cursor, bool * p_is_reference)
{
    LOG("Find definition for %s (kind: %s)\n",
        clang_getCString(clang_getCursorSpelling(cursor)),
        clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(cursor))));
    bool is_reference = false;

    /* Definition is available if the function comes from the current TU,
     * otherwise we have to look for the reference (typically in some header) */
    CXCursor definition = clang_getCursorDefinition(cursor);
    if (clang_Cursor_isNull(definition))
    {
        LOG("Looking for reference...\n");
        definition = clang_getCursorReferenced(cursor);
        is_reference = true;
    }

    //debug stuff:
    CXSourceLocation location = clang_getCursorLocation(definition);
    CXFile file;
    unsigned line;
    clang_getFileLocation(location, &file, &line, NULL, NULL);

    LOG("Found definition: %s (kind: %s) @%s:%u params: %d\n",
        clang_getCString(clang_getCursorSpelling(definition)),
        clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(definition))),
        clang_getCString(clang_getFileName(file)),
        line,
        clang_Cursor_getNumArguments(definition));

    if (p_is_reference)
    {
        *p_is_reference = is_reference;
    }
    return definition;
}

static enum CXChildVisitResult cursor_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    LOG("CHILD: %s (kind: %s, params: %d)\n",
        clang_getCString(clang_getCursorSpelling(cursor)),
        clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(cursor))),
        clang_Cursor_getNumArguments(cursor));

    return CXChildVisit_Continue;
}

static CXCursor get_function_cursor(unit_t * p_unit, const char * p_filename, unsigned line, unsigned column, unsigned * p_param_index)
{
    // Tokenize last search_lines lines
    const unsigned search_lines = 20;
    CXSourceLocation location_cursor = clang_getLocation(p_unit->tu, clang_getFile(p_unit->tu, p_filename), line, column);
    CXSourceLocation location_start = clang_getLocation(p_unit->tu,
                                                        clang_getFile(p_unit->tu, p_filename), (line > search_lines ? line - search_lines : 1),
                                                        1);
    CXSourceRange range = clang_getRange(location_start, location_cursor);
    CXToken * p_tokens = NULL;
    unsigned token_count = 0;
    CXCursor definition = clang_getNullCursor();

    clang_tokenize(p_unit->tu, range, &p_tokens, &token_count);
    if (token_count > 0)
    {
        CXCursor * p_cursors = CALLOC(token_count, sizeof(CXCursor));
        ASSERT(p_cursors);
        clang_annotateTokens(p_unit->tu, p_tokens, token_count, p_cursors);
        int parenthesises_depth = 0;
        *p_param_index = 0;
        bool found = false;
        for (int i = token_count - 1; i >= 0 && !found; --i)
        {
            enum CXCursorKind kind = clang_getCursorKind(p_cursors[i]);
            CXString token_spelling = clang_getTokenSpelling(p_unit->tu, p_tokens[i]);
            if (strcmp(clang_getCString(token_spelling), ")") == 0)
            {
                // skip casts when calculating depth
                if (kind == CXCursor_CallExpr || kind == CXCursor_FunctionDecl || kind == CXCursor_CompoundStmt)
                {
                    parenthesises_depth++;
                }
            }
            else if (strcmp(clang_getCString(token_spelling), "(") == 0)
            {
                // only account for function call when calculating
                if (kind == CXCursor_CallExpr || kind == CXCursor_FunctionDecl || kind == CXCursor_CompoundStmt)
                {
                    parenthesises_depth--;
                }
            }
            else if (parenthesises_depth == 0)
            {
                if (strcmp(clang_getCString(token_spelling), ",") == 0)
                {
                    (*p_param_index)++;
                }
            }
            else if (parenthesises_depth == -1)
            {
                /* The last token before the opening brace that puts us at -1 depth should always be the function.
                 * We should end the search here, regardless of whether we've found it or not. */
                found = true;

                if (kind == CXCursor_DeclRefExpr || kind == CXCursor_FunctionDecl || kind == CXCursor_BinaryOperator)
                {
                    definition = definition_cursor_get(p_cursors[i], NULL);
                    LOG("definition arg count: %u\n", clang_Cursor_getNumArguments(definition));
                }
            }
            LOG("TOKEN %u: %s (depth: %d, param_index: %u kind: %s, params: %d)\n",
                i,
                clang_getCString(token_spelling),
                parenthesises_depth,
                *p_param_index,
                clang_getCString(clang_getCursorKindSpelling(kind)),
                clang_Cursor_getNumArguments(p_cursors[i]));
            if (1) //kind == CXCursor_CompoundStmt)
            {
                clang_visitChildren(p_cursors[i], cursor_visitor, NULL);
            }
            clang_disposeString(token_spelling);
        }
        FREE(p_cursors);
        clang_disposeTokens(p_unit->tu, p_tokens, token_count);
    }
    else
    {
        LOG("NO TOKENS\n");
    }
    return definition;
}

static definition_t * definition_get(CXCursor definition_cursor)
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

    definition_t * p_definition = CALLOC(1, sizeof(definition_t));
    p_definition->p_name = STRDUP(clang_getCString(name));
    p_definition->p_type = STRDUP(clang_getCString(type_name));
    p_definition->p_documentation = STRDUP(clang_getCString(documentation));
    p_definition->location.text_document.uri = uri_file(clang_getCString(clang_getFileName(file)));
    p_definition->location.text_document.valid_fields = TEXT_DOCUMENT_IDENTIFIER_FIELD_ALL;
    p_definition->location.position.line = definition_line - 1;
    p_definition->location.position.character = definition_column - 1;
    p_definition->location.valid_fields = LOCATION_FIELD_ALL;
    p_definition->kind = get_kind(clang_getCursorKind(definition_cursor));

    LOG("DEFINITION URI:\n\tscheme: %s\n\tauthority: %s\n\tpath: %s\n\tquery: %s\n\tfragment: %s\n",
        p_definition->location.text_document.uri.scheme,
        p_definition->location.text_document.uri.authority,
        p_definition->location.text_document.uri.path,
        p_definition->location.text_document.uri.query,
        p_definition->location.text_document.uri.fragment);
    clang_disposeString(name);
    clang_disposeString(type_name);
    clang_disposeString(documentation);

    return p_definition;
}


static void publish_no_diags(unit_t * p_unit, const char * p_filename, unit_diagnostics_callback_t diag_callback, void * p_args)
{
    publish_diagnostics_params_t publish;
    publish.uri = uri_file(p_filename);
    publish.p_diagnostics = NULL;
    publish.diagnostics_count = 0;
    publish.valid_fields = PUBLISH_DIAGNOSTICS_PARAMS_FIELD_ALL;
    diag_callback(p_unit, &publish, p_args);
    uri_free_members(&publish.uri);
}

static void include_visitor(CXFile included_file,
                            CXSourceLocation * p_inclusion_stack,
                            unsigned include_len,
                            CXClientData client_data)
{
    inclusion_file_context_t * p_context = (inclusion_file_context_t *) client_data;
    if (!p_context->found_file)
    {
        CXString filename = clang_getFileName(included_file);
        p_context->found_file = (strcmp(clang_getCString(filename), p_context->p_filename) == 0);
        clang_disposeString(filename);
    }
}

void unit_init(const unit_config_t * p_config)
{
    m_index = clang_createIndex(0, 0);
    m_config = *p_config;
}

void unit_diagnostics_callback_set(unit_diagnostics_callback_t callback)
{
    m_diagnostics_callback = callback;
}

unit_t * unit_create(const char * p_filename,
                     const compile_flags_t * p_flags)
{
    unit_t * p_unit = MALLOC(sizeof(unit_t));

    compile_flags_clone(&p_unit->flags, p_flags);

    p_unit->diag_file_count = 0;
    p_unit->p_diag_files = NULL;
    p_unit->p_filename = normalize_path(p_filename);
    p_unit->active = false;
    p_unit->p_fixits = NULL;
    p_unit->fixit_count = 0;
    LOG("Added unit %s\n", p_unit->p_filename);
    return p_unit;
}

bool unit_parse(unit_t * p_unit,
                struct CXUnsavedFile * p_unsaved_files,
                uint32_t unsaved_file_count)
{
    ASSERT(!p_unit->active);
    p_unit->active = (clang_parseTranslationUnit2(m_index,
                                                  p_unit->p_filename,
                                                  p_unit->flags.pp_array,
                                                  p_unit->flags.count,
                                                  p_unsaved_files,
                                                  unsaved_file_count,
                                                  TRANSLATION_UNIT_PARSE_OPTIONS,
                                                  &p_unit->tu) == CXError_Success);
    return p_unit->active;
}

void unit_free(unit_t * p_unit)
{
    FREE((char *) p_unit->p_filename);
    clang_disposeTranslationUnit(p_unit->tu);
    FREE(p_unit);
}

void unit_suspend(unit_t * p_unit)
{
    clang_disposeTranslationUnit(p_unit->tu);
    p_unit->active = false;
}

bool unit_reparse(unit_t * p_unit,
                  struct CXUnsavedFile * p_unsaved_files,
                  uint32_t unsaved_file_count)
{
    ASSERT(p_unit->active);
    enum CXErrorCode status = clang_reparseTranslationUnit(p_unit->tu,
                                         unsaved_file_count,
                                         p_unsaved_files,
                                         TRANSLATION_UNIT_REPARSE_OPTIONS);
    if (status != CXError_Success)
    {
        LOG("Reparse failed: Status %u\n", status);
    }
    return (status == CXError_Success);
}

bool unit_code_completion(unit_t *p_unit,
                          const text_document_position_params_t *p_position,
                          struct CXUnsavedFile *p_unsaved_files,
                          uint32_t unsaved_file_count,
                          unit_completion_result_callback_t callback,
                          void *p_args)
{

    ASSERT(p_unit);
    ASSERT(p_position);
    ASSERT(callback);
    ASSERT(p_unit->active);

    CXCodeCompleteResults * p_results = clang_codeCompleteAt(p_unit->tu,
                                                             p_position->text_document.uri.path,
                                                             (unsigned) p_position->position.line + 1,
                                                             (unsigned) p_position->position.character + 1,
                                                             p_unsaved_files,
                                                             unsaved_file_count,
                                                             COMPLETION_FLAGS);

    if (p_results)
    {
        unsigned result_count = p_results->NumResults;
        for (unsigned i = 0; i < min(result_count, m_config.completion_results_max); ++i)
        {
            CXCompletionString completion = p_results->Results[i].CompletionString;

            /* Punish functions with poor availability */
            uint32_t priority = clang_getCompletionPriority(completion);// + 10000 * clang_getCompletionAvailability(completion);
            if (priority > m_config.completion_priority_max)
            {
                LOG("Discarded completion item with priority %u\n", priority);
                continue;
            }

            CXString doc = clang_getCompletionBriefComment(completion);
            CXString typed_text;
            bool has_typed_text = false;
            CXString inserted_text;
            bool has_inserted_text = false;
            CXString var_type_text;
            bool has_var_type_text = false;

            char * p_full_text = MALLOC(COMPLETION_STRING_MAXLEN);
            char * p_full_text_next = p_full_text;
            unsigned placeholders = 0;
            unsigned chunk_count = clang_getNumCompletionChunks(completion);

            for (unsigned j = 0; j < chunk_count; ++j)
            {
                enum CXCompletionChunkKind kind = clang_getCompletionChunkKind(completion, j);
                CXString chunk_string           = clang_getCompletionChunkText(completion, j);

                const char * p_text = clang_getCString(chunk_string);
                unsigned text_len = strlen(p_text);

                if (false && kind == CXCompletionChunk_Placeholder)
                {
                    if (p_full_text_next + text_len + 8 < p_full_text + COMPLETION_STRING_MAXLEN)
                    {
                        p_full_text_next += sprintf(p_full_text_next, "${%u:", ++placeholders);
                        strcpy(p_full_text_next, p_text);
                        p_full_text_next += text_len;
                        p_full_text_next += sprintf(p_full_text_next, "}");
                    }
                }
                else if (kind != CXCompletionChunk_ResultType && kind != CXCompletionChunk_Optional) //TODO: Emit separate results per optional chunk
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
                        has_typed_text = true;
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


            completion_item_t result;
            result.valid_fields = (COMPLETION_ITEM_FIELD_KIND |
                                   COMPLETION_ITEM_FIELD_LABEL |
                                   COMPLETION_ITEM_FIELD_SORT_TEXT |
                                   COMPLETION_ITEM_FIELD_INSERT_TEXT_FORMAT);
            result.kind = get_kind(p_results->Results[i].CursorKind);
            if (has_typed_text)
            {
                result.insert_text = (char *) clang_getCString(typed_text);
                result.valid_fields |= COMPLETION_ITEM_FIELD_INSERT_TEXT;
                result.filter_text = (char *) clang_getCString(typed_text);
                result.valid_fields |= COMPLETION_ITEM_FIELD_FILTER_TEXT;
            }
            else
            {
                result.insert_text = "";
            }

            if (has_var_type_text)
            {
                result.detail = (char *) clang_getCString(var_type_text);
                result.valid_fields |= COMPLETION_ITEM_FIELD_DETAIL;
            }

            result.label = p_full_text;
            char * p_markdown_doc = doxygen_to_markdown(clang_getCString(doc), false);
            if (p_markdown_doc)
            {
                result.documentation.kind = MARKUP_KIND_MARKUP;
                result.documentation.value = p_markdown_doc;
                result.documentation.valid_fields = MARKUP_CONTENT_FIELD_ALL;
                result.valid_fields |= COMPLETION_ITEM_FIELD_DOCUMENTATION;
            }

            /* Make a string of priority + name to get prioritized sorting, e.g. "000071 param" */
            char * p_sort_text_buf = CALLOC(strlen(result.insert_text) + 8, 1);
            sprintf(p_sort_text_buf, "%06u %s", priority, result.insert_text);
            result.sort_text = p_sort_text_buf;

            if (has_inserted_text)
            {
                // result.insert_text = STRDUP(clang_getCString(inserted_text));
                // result.valid_fields |= COMPLETION_ITEM_FIELD_INSERT_TEXT;
            }
            if (placeholders > 0)
            {
                result.insert_text_format = INSERT_TEXT_FORMAT_SNIPPET;
                if (!has_inserted_text)
                {
                    result.insert_text = p_full_text;
                    result.valid_fields |= COMPLETION_ITEM_FIELD_INSERT_TEXT;
                }
            }
            else
            {
                result.insert_text_format = INSERT_TEXT_FORMAT_PLAIN_TEXT;
            }

            // LOG("GOT KIND %u: %s\n", result.kind, p_full_text);
            const char * p_commit_chars_function[] = {"("};

            if (result.kind == COMPLETION_ITEM_KIND_FUNCTION)
            {
                /* For functions, commit the completion when the user types "(" */
                result.commit_characters_count = ARRAY_SIZE(p_commit_chars_function);
                result.p_commit_characters = p_commit_chars_function;
                result.valid_fields |= COMPLETION_ITEM_FIELD_COMMIT_CHARACTERS;
            }

            callback(&result, result_count, p_args);

            FREE(p_full_text);
            FREE(p_markdown_doc);
            clang_disposeString(doc);
            clang_disposeString(typed_text);
            FREE(p_sort_text_buf);

            if (has_typed_text)
            {
                clang_disposeString(typed_text);
            }
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
        return (result_count <= m_config.completion_results_max);
    }
    else
    {
        return true;
    }
}

unsigned unit_function_signature_get(unit_t *p_unit,
                                     const text_document_position_params_t *p_position,
                                     unit_signature_callback_t callback,
                                     void *p_args)
{
    ASSERT(p_unit);
    ASSERT(p_position);
    ASSERT(callback);
    ASSERT(p_unit->active);

    unsigned param_index = 0;
    CXCursor definition_cursor = get_function_cursor(p_unit,
                                                     p_position->text_document.uri.path,
                                                     p_position->position.line + 1,
                                                     p_position->position.character + 1,
                                                     &param_index);
    int arg_count = clang_Cursor_getNumArguments(definition_cursor);

    if (arg_count >= 0)
    {
        signature_information_t info;
        memset(&info, 0, sizeof(signature_information_t));
        info.valid_fields = SIGNATURE_INFORMATION_FIELD_LABEL;

        definition_t * p_definition = definition_get(definition_cursor);
        if (p_definition)
        {
            doxygen_function_t * p_doxygen = NULL;

            if (p_definition->p_documentation)
            {
                p_doxygen = doxygen_function_parse(p_definition->p_documentation);
                const char * p_doc_markdown = doxygen_function_to_markdown(p_doxygen, false, true);

                if (false && p_doc_markdown)
                {
                    info.documentation.kind = STRDUP(MARKUP_KIND_MARKUP);
                    info.documentation.value = p_doc_markdown;
                    info.documentation.valid_fields = MARKUP_CONTENT_FIELD_ALL;
                    info.valid_fields |= SIGNATURE_INFORMATION_FIELD_DOCUMENTATION;
                }
            }
            info.label = MALLOC(1024);
            char * p_label_next = info.label + sprintf(info.label, "%s(", p_definition->p_name);

            if (arg_count > 0)
            {
                info.p_parameters = CALLOC(arg_count, sizeof(parameter_information_t));
                info.parameters_count = arg_count;
                info.valid_fields |= SIGNATURE_INFORMATION_FIELD_PARAMETERS;

                for (unsigned i = 0; i < (unsigned) arg_count; ++i)
                {
                    CXCursor arg = clang_Cursor_getArgument(definition_cursor, i);
                    CXType type = clang_getCursorType(arg);

                    CXString type_name = clang_getTypeSpelling(type);
                    CXString name = clang_getCursorDisplayName(arg);

                    if (i != 0)
                    {
                        p_label_next += sprintf(p_label_next, ", ");
                    }
                    p_label_next += sprintf(p_label_next, "%s %s", clang_getCString(type_name), clang_getCString(name));

                    info.p_parameters[i].valid_fields = PARAMETER_INFORMATION_FIELD_LABEL;
                    info.p_parameters[i].label = MALLOC(strlen(clang_getCString(type_name)) + strlen(clang_getCString(name)) + 2);
                    sprintf(info.p_parameters[i].label, "%s %s", clang_getCString(type_name), clang_getCString(name));

                    if (p_doxygen)
                    {
                        /* Match doxygen argument list by name, not by index. */
                        const doxygen_arg_t * p_doxygen_arg = NULL;
                        for (unsigned j = 0; j < p_doxygen->arg_count; ++j)
                        {
                            if (strcmp(clang_getCString(name), p_doxygen->p_args[j].p_name) == 0)
                            {
                                p_doxygen_arg = &p_doxygen->p_args[j];
                                break;
                            }
                        }

                        if (p_doxygen_arg)
                        {
                            char * p_description = doxygen_to_markdown(p_doxygen_arg->description, true);
                            // char * p_arg_doc = MALLOC(1 + strlen(clang_getCString(name)) + 3 + strlen(p_description) + 2);
                            // sprintf(p_arg_doc, "`%s`: %s\n", clang_getCString(name), p_description);
                            // FREE(p_description);
                            info.p_parameters[i].documentation.kind = STRDUP(MARKUP_KIND_MARKUP);
                            info.p_parameters[i].documentation.value = p_description; //p_arg_doc;
                            info.p_parameters[i].documentation.valid_fields = MARKUP_CONTENT_FIELD_ALL;
                            info.p_parameters[i].valid_fields |= SIGNATURE_INFORMATION_FIELD_DOCUMENTATION;
                        }
                    }

                    clang_disposeString(type_name);
                    clang_disposeString(name);
                }
            }

            sprintf(p_label_next, ")");

            callback(&info, 0, p_args);

            free_signature_information(info);
            doxygen_function_free(p_doxygen);
            FREE(p_definition->p_documentation);
            FREE(p_definition->p_name);
            FREE(p_definition->p_type);
            FREE((void *) p_definition);
        }
        else
        {
            LOG("Couldn't parse definition of the function at the given position\n");
        }
    }
    else
    {
        LOG("Couldn't find a valid function at the given position\n");
    }

    return param_index;
}


void unit_definition_get(unit_t *p_unit,
                         const text_document_position_params_t *p_position,
                         unit_definition_callback_t callback,
                         void *p_args)
{
    ASSERT(p_unit);
    ASSERT(callback);
    ASSERT(p_unit->active);

    CXCursor cursor = cursor_get(p_unit,
                                 p_position->text_document.uri.path,
                                 p_position->position.line + 1,
                                 p_position->position.character + 1);

    bool is_reference;
    CXCursor definition_cursor = definition_cursor_get(cursor, &is_reference);
    LOG("USR: %s\n", clang_getCString(clang_getCursorUSR(definition_cursor)));

    if (!clang_Cursor_isNull(definition_cursor)) // && !clang_equalCursors(cursor, definition_cursor))
    {
        LOG("Got definition cursor\n");
        definition_t * p_definition = definition_get(definition_cursor);
        if (p_definition)
        {
            location_t location;
            location.uri = p_definition->location.text_document.uri;
            location.range.start.character = p_definition->location.position.character;
            location.range.start.line = p_definition->location.position.line;
            location.range.start.valid_fields = POSITION_FIELD_ALL;
            location.range.end.character = p_definition->location.position.character;
            location.range.end.line = p_definition->location.position.line;
            location.range.end.valid_fields = POSITION_FIELD_ALL;
            location.range.valid_fields = RANGE_FIELD_ALL;
            location.valid_fields = LOCATION_FIELD_ALL;

            callback(&location, 0, p_args, is_reference ? DEFINITION_TYPE_REFERENCE : DEFINITION_TYPE_DEFINITION);

            FREE(p_definition->p_documentation);
            FREE(p_definition->p_name);
            FREE(p_definition->p_type);
            uri_free_members(&p_definition->location.text_document.uri);
            FREE((void *) p_definition);
        }
    }
}

bool unit_hover_get(unit_t * p_unit,
                    const text_document_position_params_t * p_position,
                    hover_t * p_hover)
{
    ASSERT(p_unit);
    ASSERT(p_position);
    ASSERT(p_hover);
    ASSERT(p_unit->active);

    CXCursor cursor = cursor_get(p_unit,
                                 p_position->text_document.uri.path,
                                 p_position->position.line + 1,
                                 p_position->position.character + 1);

    bool is_reference;
    CXCursor definition_cursor = definition_cursor_get(cursor, &is_reference);

    if (!clang_Cursor_isNull(definition_cursor))
    {
        LOG("Got definition cursor\n");
        CXString name = clang_getCursorDisplayName(definition_cursor);
        LOG("Display name: %s\n", clang_getCString(name));

        memset(p_hover, 0, sizeof(hover_t));
        p_hover->range.start.valid_fields = POSITION_FIELD_ALL;
        p_hover->range.end.valid_fields = POSITION_FIELD_ALL;
        p_hover->range.valid_fields = RANGE_FIELD_ALL;
        p_hover->contents.valid_fields = MARKUP_CONTENT_FIELD_ALL;
        p_hover->valid_fields = HOVER_FIELD_ALL;

        CXSourceRange clang_range = clang_getCursorExtent(cursor);
        CXSourceLocation start = clang_getRangeStart(clang_range);
        CXSourceLocation end = clang_getRangeEnd(clang_range);
        unsigned start_line, start_character;
        unsigned end_line, end_character;
        clang_getSpellingLocation(start, NULL, &start_line, &start_character, NULL);
        clang_getSpellingLocation(end, NULL, &end_line, &end_character, NULL);

        p_hover->range.start.line = start_line - 1;
        p_hover->range.start.character = start_character - 1;
        p_hover->range.end.line = end_line - 1;
        p_hover->range.end.character = end_character - 1;

        const char * p_header = "\n\n```c\n";
        const char * p_footer = "\n```\n";
        char * p_type = NULL;
        CXString doc_string = clang_Cursor_getBriefCommentText(definition_cursor);

        const char * p_doc_string = clang_getCString(doc_string);

        enum CXCursorKind kind = clang_getCursorKind(definition_cursor);
        CXType type = clang_getCursorType(definition_cursor);
        if (kind == CXCursor_FunctionDecl)
        {
            CXString type_string = clang_getTypeSpelling(clang_getResultType(type));
            p_type = STRDUP(clang_getCString(type_string));
            clang_disposeString(type_string);
        }
        else
        {
            CXString type_string = clang_getTypeSpelling(type);
            p_type = STRDUP(clang_getCString(type_string));
            clang_disposeString(type_string);
        }
        LOG("KIND: %s\n", clang_getCString(clang_getTypeKindSpelling(kind)));

        size_t str_length = strlen(p_header) + strlen(p_type) + strlen(clang_getCString(name)) + strlen(p_footer) + 2;
        if (p_doc_string)
        {
            str_length += strlen(p_doc_string);
        }

        p_hover->contents.value = malloc(str_length);
        ASSERT(p_hover->contents.value);
        sprintf(p_hover->contents.value, "%s%s%s %s%s", p_doc_string ? p_doc_string : "" , p_header, p_type, clang_getCString(name), p_footer);
        p_hover->contents.kind = STRDUP(MARKUP_KIND_MARKUP);
        clang_disposeString(name);
        clang_disposeString(doc_string);
        free(p_type);
        return true;
    }
    return false;
}

unsigned unit_diagnostics_get(unit_t *p_unit,
                              unit_diagnostics_callback_t diag_callback,
                              unit_fixit_callback_t fixit_callback,
                              void *p_args)
{
    ASSERT(p_unit);
    ASSERT(diag_callback);
    ASSERT(p_unit->active);

    CXDiagnosticSet set = clang_getDiagnosticSetFromTU(p_unit->tu);
    unsigned count = clang_getNumDiagnosticsInSet(set);

    if (count > 0)
    {
        /* The diags come out of Clang in a bunch, while LSP requires us to report one list per file.
         * Gather one set per file, and assign each Clang diagnostic to their matching file set. */
        struct
        {
            CXFile file;
            diagnostic_t * p_diagnostics;
            unsigned diag_count;
        } diag_file[DIAG_MAX_FILES];

        unsigned file_count = 0;
        unsigned total_fixit_count = 0;
        unsigned fixit_index = 0;

        for (unsigned i = 0; i < count; ++i)
        {
            CXDiagnostic clang_diag = clang_getDiagnosticInSet(set, i);
            total_fixit_count += clang_getDiagnosticNumFixIts(clang_diag);
            clang_disposeDiagnostic(clang_diag);
        }

        if (total_fixit_count > 0)
        {
            if (p_unit->p_fixits)
            {
                for (unsigned i = 0; i < p_unit->fixit_count; ++i)
                {
                    free(p_unit->p_fixits[i].p_filename);
                    free(p_unit->p_fixits[i].p_string);
                }
            }

            p_unit->fixit_count = total_fixit_count;
            p_unit->p_fixits = REALLOC(p_unit->p_fixits, sizeof(fixit_t) * total_fixit_count);
        }

        for (unsigned i = 0; i < count; ++i)
        {
            CXDiagnostic clang_diag = clang_getDiagnosticInSet(set, i);
            CXFile file;
            diagnostic_t diag;
            if (diagnostic_parse(clang_diag, &diag, &file))
            {
                bool found_existing_set = false;
                /* Find the diag file this diag belongs to. Start searching at the back, as
                 * diagnostics are likely to be returned from one file at the time. */
                for (int j = file_count - 1; j >= 0; --j)
                {
                    if (clang_File_isEqual(file, diag_file[j].file))
                    {
                        if (diag_file[j].diag_count < DIAG_MAX_PER_FILE)
                        {
                            diag_file[j].p_diagnostics[diag_file[j].diag_count++] = diag;
                        }
                        found_existing_set = true;
                        break;
                    }
                }
                if (!found_existing_set && file_count < DIAG_MAX_FILES)
                {
                    diag_file[file_count].p_diagnostics = MALLOC(sizeof(diagnostic_t) * DIAG_MAX_PER_FILE);
                    ASSERT(diag_file[file_count].p_diagnostics);

                    diag_file[file_count].p_diagnostics[0] = diag;
                    diag_file[file_count].diag_count = 1;
                    diag_file[file_count].file = file;
                    file_count++;
                }
            }

            unsigned num_fixits = clang_getDiagnosticNumFixIts(clang_diag);

            for (unsigned j = 0; j < num_fixits; ++j)
            {
                CXSourceRange replacement_range;
                CXString string = clang_getDiagnosticFixIt(clang_diag, j, &replacement_range);

                CXSourceLocation start = clang_getRangeStart(replacement_range);
                CXSourceLocation end = clang_getRangeEnd(replacement_range);
                unsigned start_line, start_character;
                unsigned end_line, end_character;
                CXFile file;
                clang_getSpellingLocation(start, &file, &start_line, &start_character, NULL);
                clang_getSpellingLocation(end, NULL, &end_line, &end_character, NULL);

                CXString filename = clang_getFileName(file);
                fixit_t * p_fixit = &p_unit->p_fixits[fixit_index++];
                p_fixit->p_filename = STRDUP(clang_getCString(filename));
                p_fixit->p_string = STRDUP(clang_getCString(string));
                p_fixit->range.start.line = start_line - 1;
                p_fixit->range.start.character = start_character - 1;
                p_fixit->range.start.valid_fields = POSITION_FIELD_ALL;
                p_fixit->range.end.line = end_line - 1;
                p_fixit->range.end.character = end_character - 1;
                p_fixit->range.end.valid_fields = POSITION_FIELD_ALL;
                p_fixit->range.valid_fields = RANGE_FIELD_ALL;

                clang_disposeString(filename);
                clang_disposeString(string);
            }
            clang_disposeDiagnostic(clang_diag);
        }
        char ** p_filenames = MALLOC(sizeof(char *) * file_count);
        /* Report once per file */
        for (unsigned i = 0; i < file_count; ++i)
        {
            publish_diagnostics_params_t publish;
            if (diag_file[i].file)
            {
                CXString filename = clang_getFileName(diag_file[i].file);
                publish.uri = uri_file(clang_getCString(filename));
                p_filenames[i] = STRDUP(publish.uri.path);
                clang_disposeString(filename);
            }
            else
            {
                publish.uri = uri_decode("clang-server:///command-line");
                p_filenames[i] = STRDUP(publish.uri.path);
            }
            publish.p_diagnostics = diag_file[i].p_diagnostics;
            publish.diagnostics_count = diag_file[i].diag_count;
            publish.valid_fields = PUBLISH_DIAGNOSTICS_PARAMS_FIELD_ALL;
            diag_callback(p_unit, &publish, p_args);

            for (unsigned j = 0; j < diag_file[i].diag_count; ++j)
            {
                FREE(diag_file[i].p_diagnostics[j].message);
            }
            FREE(diag_file[i].p_diagnostics);
            uri_free_members(&publish.uri);
        }

        if (p_unit->p_diag_files)
        {
            for (unsigned i = 0; i < p_unit->diag_file_count; ++i)
            {
                bool still_has_file = false;
                for (unsigned j = 0; j < file_count; ++j)
                {
                    if (strcmp(p_unit->p_diag_files[i], p_filenames[j]) == 0)
                    {
                        still_has_file = true;
                        break;
                    }
                }
                if (!still_has_file)
                {
                    publish_no_diags(p_unit, p_unit->p_diag_files[i], diag_callback, p_args);
                }
                FREE(p_unit->p_diag_files[i]);
            }
            FREE(p_unit->p_diag_files);
        }

        p_unit->p_diag_files = p_filenames;
        p_unit->diag_file_count = file_count;
    }
    else
    {
        if (p_unit->p_diag_files)
        {
            for (unsigned i = 0; i < p_unit->diag_file_count; ++i)
            {
                publish_no_diags(p_unit, p_unit->p_diag_files[i], diag_callback, p_args);
                FREE(p_unit->p_diag_files[i]);
            }
            FREE(p_unit->p_diag_files);
        }
        p_unit->p_diag_files = NULL;
        p_unit->diag_file_count = 0;
    }

    clang_disposeDiagnosticSet(set);
    return count;
}

void unit_fixits_resolve(unit_t * p_unit, const char * p_filename, const range_t * p_range, command_t * p_commands, unsigned * p_count)
{
    unsigned max_count = min(*p_count, p_unit->fixit_count);
    unsigned count = 0;
    LOG("Resolving fixits for %s (max_count: %u)\n", p_filename, max_count);
    for (unsigned i = 0; i < max_count; ++i)
    {
        LOG("\tFound fixit in file %s:%lld:%lld: %s (range: %lld:%lld-%lld:%lld)\n",
            p_unit->p_fixits[i].p_filename,
            p_unit->p_fixits[i].range.start.line,
            p_unit->p_fixits[i].range.start.character,
            p_unit->p_fixits[i].p_string,
            p_range->start.line,
            p_range->start.character,
            p_range->end.line,
            p_range->end.character);

        if (path_equals(p_filename, p_unit->p_fixits[i].p_filename) && range_overlap(p_range, &p_unit->p_fixits[i].range))
        {
            p_commands[count].command = "clang-server.fixit";
            const char * p_header;
            if (range_is_zero_length(&p_unit->p_fixits[i].range))
            {
                p_header = "Fix problem: Insert ";
            }
            else
            {
                p_header = "Fix problem: Replace with ";
            }

            p_commands[count].title = malloc(strlen(p_header) + strlen(p_unit->p_fixits[i].p_string) + 1);
            sprintf(p_commands[count].title, "%s%s", p_header, p_unit->p_fixits[i].p_string);
            p_commands[count].arguments = json_array();
            json_array_append_new(p_commands[count].arguments, json_string(p_unit->p_fixits[i].p_string));
            json_array_append_new(p_commands[count].arguments, encode_range(p_unit->p_fixits[i].range));
            json_array_append_new(p_commands[count].arguments, json_string(p_filename));
            p_commands[count].valid_fields = COMMAND_FIELD_ALL;
            count++;
        }
    }
    LOG("Resolved.\n");
    *p_count = count;
}

bool unit_includes_file(unit_t * p_unit, const char * p_file)
{
    inclusion_file_context_t context =
    {
        false,
        p_file
    };
    clang_getInclusions(p_unit->tu, include_visitor, &context);
    return context.found_file;
}