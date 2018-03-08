#include <stdio.h>
#include "clang-c/Index.h"
#define TEST_FILE "c:/Users/Trond/dev/vscode_extensions/clang-server/backend/test/target_files/test.c"

static const char * mp_chunk_kinds[] = {
    "Optional        ",
    "TypedText       ",
    "Text            ",
    "Placeholder     ",
    "Informative     ",
    "CurrentParameter",
    "LeftParen       ",
    "RightParen      ",
    "LeftBracket     ",
    "RightBracket    ",
    "LeftBrace       ",
    "RightBrace      ",
    "LeftAngle       ",
    "RightAngle      ",
    "Comma           ",
    "ResultType      ",
    "Colon           ",
    "SemiColon       ",
    "Equal           ",
    "HorizontalSpace ",
    "VerticalSpace   "
};


#define COMPLETION 1
#define INCLUDEFILES 2
#define SYMBOLLOOKUP 3

#define ACTION COMPLETION

void visitor(CXFile included_file,
             CXSourceLocation *inclusion_stack,
             unsigned include_len,
             CXClientData client_data)
{
    printf("INCLUDED: %s\n", clang_getCString(clang_getFileName(included_file)));
}

int main(char ** argv, int argc)
{
    CXIndex index = clang_createIndex(0, 1);
    CXTranslationUnit tu = clang_parseTranslationUnit(index, TEST_FILE, NULL, 0, NULL, 0, CXTranslationUnit_IncludeBriefCommentsInCodeCompletion | CXTranslationUnit_KeepGoing | CXTranslationUnit_CacheCompletionResults);
    if (tu == NULL)
    {
        printf("No translation unit\n");
    }
    else
    {
#if (ACTION == COMPLETION)
        CXCodeCompleteResults * p_results = clang_codeCompleteAt(tu, TEST_FILE, 51, 12, NULL, 0, clang_defaultCodeCompleteOptions() | CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeBriefComments);
        printf("Got %d results:\n", p_results->NumResults);
        clang_sortCodeCompletionResults(p_results->Results, p_results->NumResults);
        for (unsigned i = 0; i < p_results->NumResults; ++i)
        {
            printf("%u (%4u): ", i, p_results->Results[i].CursorKind);

            unsigned chunks = clang_getNumCompletionChunks(p_results->Results[i].CompletionString);
            for (unsigned j = 0; j < chunks; j++)
            {
                enum CXCompletionChunkKind kind = clang_getCompletionChunkKind(p_results->Results[i].CompletionString, j);
                if (kind == CXCompletionChunk_Text || kind == CXCompletionChunk_TypedText)
                    printf("\033[31m");

                printf("%s", clang_getCString(clang_getCompletionChunkText(p_results->Results[i].CompletionString, j)));
                if (kind == CXCompletionChunk_ResultType)
                    printf(" ");
                printf("\033[0m");
            }
            const char * docstring = clang_getCString(clang_getCompletionBriefComment(p_results->Results[i].CompletionString));
            if (docstring)
                printf(" /**< %s */", docstring);
            printf("\n");
        }
#elif (ACTION == INCLUDEFILES)
        clang_getInclusions(tu, visitor, NULL);
#elif (ACTION == SYMBOLLOOKUP)
        CXSourceLocation location = clang_getLocation(tu, clang_getFile(tu, TEST_FILE), 49, 13);
        CXCursor cursor = clang_getCursor(tu, location);

        printf("CURSOR: %s, completion: %s (%u)\n", clang_getCString(clang_getCursorDisplayName(cursor)), clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(cursor))), clang_getCursorKind(cursor));
        CXCursor type_cursor = clang_getTypeDeclaration(clang_getCursorType(cursor));
        CXCursor definition_cursor = clang_getCursorDefinition(cursor);
        CXType definition_type = clang_getCursorType(definition_cursor);
        printf("DEFINITION TYPE: %s\n", clang_getCString(clang_getTypeSpelling(definition_type)));

        printf("IS MACRO: original: %u\n", clang_Cursor_isMacroFunctionLike(type_cursor));

        int args = clang_Cursor_getNumArguments(definition_cursor);
        if (args > 0) {
            for (unsigned i = 0; i < args; ++i)
            {
                CXCursor arg = clang_Cursor_getArgument(definition_cursor, i);
                printf("ARGS: %s %s\n", clang_getCString(clang_getTypeSpelling(clang_getCursorType(arg))), clang_getCString(clang_getCursorSpelling(arg)));
            }
        }
        struct
        {
            CXFile file;
            unsigned line;
            unsigned column;
        } type;
        clang_getExpansionLocation(location, &type.file, &type.line, &type.column, NULL);
        printf("ORIGINAL: %s:%u:%u (%s)\n", clang_getCString(clang_getFileName(type.file)), type.line, type.column, clang_getCString(clang_getCursorSpelling(cursor)));
        clang_getExpansionLocation(clang_getCursorLocation(type_cursor), &type.file, &type.line, &type.column, NULL);
        printf("TYPE: %s:%u:%u\n", clang_getCString(clang_getFileName(type.file)), type.line, type.column);
        clang_getExpansionLocation(clang_getCursorLocation(definition_cursor), &type.file, &type.line, &type.column, NULL);
        printf("DEFINITION: %s:%u:%u (%s)\n", clang_getCString(clang_getFileName(type.file)), type.line, type.column, clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(definition_cursor))));

#endif
    }
    return 0;
}