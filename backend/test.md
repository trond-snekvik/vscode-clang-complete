
Annotate the given set of tokens by providing cursors for each token
that can be mapped to a specific entity within the abstract syntax tree.


**Details:**

This token-annotation routine is equivalent to invoking
clang_getCursor() for the source locations of each of the
tokens. The cursors provided are filtered, so that only those
cursors that have a direct correspondence to the token are
accepted. For example, given a function call `f(x),`
clang_getCursor() would provide the following cursors:

  * when the cursor is over the 'f', a DeclRefExpr cursor referring to 'f'.
  * when the cursor is over the '(' or the ')', a CallExpr referring to 'f'.
  * when the cursor is over the 'x', a DeclRefExpr cursor referring to 'x'.

Only the first and last of these cursors will occur within the
annotate, since the tokens "f" and "x' directly refer to a function
and a variable, respectively, but the parentheses are just a small
part of the full syntax of the function call expression, which is
not provided as an annotation.


| Parameter | Description
|-----------|--------------
|  | `TU` | the translation unit that owns the given tokens.
|  | `Tokens` | the set of tokens to annotate.
|  | `NumTokens` | the number of tokens in `Tokens.`
|  | `Cursors` | an array of `NumTokens` cursors, whose contents will be replaced with the cursors corresponding to each token.

---

Annotate the given set of tokens by providing cursors for each token
that can be mapped to a specific entity within the abstract syntax tree.


**Details:**

This token-annotation routine is equivalent to invoking
clang_getCursor() for the source locations of each of the
tokens. The cursors provided are filtered, so that only those
cursors that have a direct correspondence to the token are
accepted. For example, given a function call `f(x),`
clang_getCursor() would provide the following cursors:

  * when the cursor is over the 'f', a DeclRefExpr cursor referring to 'f'.
  * when the cursor is over the '(' or the ')', a CallExpr referring to 'f'.
  * when the cursor is over the 'x', a DeclRefExpr cursor referring to 'x'.

Only the first and last of these cursors will occur within the
annotate, since the tokens "f" and "x' directly refer to a function
and a variable, respectively, but the parentheses are just a small
part of the full syntax of the function call expression, which is
not provided as an annotation.

| Parameter | Description
|-----------|--------------
| `TU` | the translation unit that owns the given tokens.
| `Tokens` | the set of tokens to annotate.
| `NumTokens` | the number of tokens in `Tokens.`
| `Cursors` | an array of `NumTokens` cursors, whose contents will be replaced with the cursors corresponding to each token.

---
Annotate the given set of tokens by providing cursors for each token
that can be mapped to a specific entity within the abstract syntax tree.




**Details:**

This token-annotation routine is equivalent to invoking
clang_getCursor() for the source locations of each of the
tokens. The cursors provided are filtered, so that only those
cursors that have a direct correspondence to the token are
accepted. For example, given a function call `f(x),`
clang_getCursor() would provide the following cursors:

  * when the cursor is over the 'f', a DeclRefExpr cursor referring to 'f'.
  * when the cursor is over the '(' or the ')', a CallExpr referring to 'f'.
  * when the cursor is over the 'x', a DeclRefExpr cursor referring to 'x'.

Only the first and last of these cursors will occur within the
annotate, since the tokens "f" and "x' directly refer to a function
and a variable, respectively, but the parentheses are just a small
part of the full syntax of the function call expression, which is
not provided as an annotation.

| Parameter | Description
|-------------|-----------|--------------
| `TU` | the translation unit that owns the given tokens.
| `Tokens` | the set of tokens to annotate.
| `NumTokens` | the number of tokens in `Tokens.`
| `Cursors` | an array of `NumTokens` cursors, whose contents will be replaced with the cursors corresponding to each token.


---
Annotate the given set of tokens by providing cursors for each token
that can be mapped to a specific entity within the abstract syntax tree.




**Details:**

This token-annotation routine is equivalent to invoking
clang_getCursor() for the source locations of each of the
tokens. The cursors provided are filtered, so that only those
cursors that have a direct correspondence to the token are
accepted. For example, given a function call `f(x),`
clang_getCursor() would provide the following cursors:

  * when the cursor is over the 'f', a DeclRefExpr cursor referring to 'f'.
  * when the cursor is over the '(' or the ')', a CallExpr referring to 'f'.
  * when the cursor is over the 'x', a DeclRefExpr cursor referring to 'x'.

Only the first and last of these cursors will occur within the
annotate, since the tokens "f" and "x' directly refer to a function
and a variable, respectively, but the parentheses are just a small
part of the full syntax of the function call expression, which is
not provided as an annotation.

| Direction   | Parameter | Description
|-------------|-----------|--------------
| `in` | `TU` | the translation unit that owns the given tokens.
| `in, out` | `Tokens` | the set of tokens to annotate.
| `out` | `NumTokens` | the number of tokens in `Tokens.`
|  | `Cursors` | an array of `NumTokens` cursors, whose contents will be replaced with the cursors corresponding to each token.


---
Annotate the given set of tokens by providing cursors for each token
that can be mapped to a specific entity within the abstract syntax tree.




**Details:**

This token-annotation routine is equivalent to invoking
clang_getCursor() for the source locations of each of the
tokens. The cursors provided are filtered, so that only those
cursors that have a direct correspondence to the token are
accepted. For example, given a function call `f(x),`
clang_getCursor() would provide the following cursors:

  * when the cursor is over the 'f', a DeclRefExpr cursor referring to 'f'.
  * when the cursor is over the '(' or the ')', a CallExpr referring to 'f'.
  * when the cursor is over the 'x', a DeclRefExpr cursor referring to 'x'.

Only the first and last of these cursors will occur within the
annotate, since the tokens "f" and "x' directly refer to a function
and a variable, respectively, but the parentheses are just a small
part of the full syntax of the function call expression, which is
not provided as an annotation.

| Direction | Parameter | Description
|-------------|-----------|--------------
| `in` | `TU` | the translation unit that owns the given tokens.
| `in, out` | `Tokens` | the set of tokens to annotate.
| `out` | `NumTokens` | the number of tokens in `Tokens.`
|  | `Cursors` | an array of `NumTokens` cursors, whose contents will be replaced with the cursors corresponding to each token.

| Return description
|--------------------------------------


---
Annotate the given set of tokens by providing cursors for each token
that can be mapped to a specific entity within the abstract syntax tree.




**Details:**

This token-annotation routine is equivalent to invoking
clang_getCursor() for the source locations of each of the
tokens. The cursors provided are filtered, so that only those
cursors that have a direct correspondence to the token are
accepted. For example, given a function call `f(x),`
clang_getCursor() would provide the following cursors:

  * when the cursor is over the 'f', a DeclRefExpr cursor referring to 'f'.
  * when the cursor is over the '(' or the ')', a CallExpr referring to 'f'.
  * when the cursor is over the 'x', a DeclRefExpr cursor referring to 'x'.

Only the first and last of these cursors will occur within the
annotate, since the tokens "f" and "x' directly refer to a function
and a variable, respectively, but the parentheses are just a small
part of the full syntax of the function call expression, which is
not provided as an annotation.

| Direction | Parameter | Description
|-------------|-----------|--------------
| `in` | `TU` | the translation unit that owns the given tokens.
| `in, out` | `Tokens` | the set of tokens to annotate.
| `out` | `NumTokens` | the number of tokens in `Tokens.`
|  | `Cursors` | an array of `NumTokens` cursors, whose contents will be replaced with the cursors corresponding to each token.


---
The kind of entity that this completion refers to.




**Details:**

The cursor kind will be a macro, keyword, or a declaration (one of the
*Decl cursor kinds), describing the entity that the completion is
referring to.



> TODO: In the future, we would like to provide a full cursor, to allow
the client to extract additional information from declaration.





---
The kind of entity that this completion refers to.




**Details:**

The cursor kind will be a macro, keyword, or a declaration (one of the
*Decl cursor kinds), describing the entity that the completion is
referring to.



> **TODO:** In the future, we would like to provide a full cursor, to allow
the client to extract additional information from declaration.





---
The kind of entity that this completion refers to.




**Details:**

The cursor kind will be a macro, keyword, or a declaration (one of the
*Decl cursor kinds), describing the entity that the completion is
referring to.



> **TODO:** In the future, we would like to provide a full cursor, to allow
the client to extract additional information from declaration.





---
The kind of entity that this completion refers to.




**Details:**

The cursor kind will be a macro, keyword, or a declaration (one of the
*Decl cursor kinds), describing the entity that the completion is
referring to.



> **TODO:** In the future, we would like to provide a full cursor, to allow
the client to extract additional information from declaration.





---
