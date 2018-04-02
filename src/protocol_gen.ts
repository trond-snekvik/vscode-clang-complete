import * as fs from 'fs';


interface Structure {
    name: string;
    members: any;
    required: string[];
}

interface Request {
    name: string;
    parameters: string;
    response: string;
}

interface Notification {
    name: string;
    parameters: string;
}


interface Database {
    defines: any;
    common: Structure[];
    server_command_params: Structure[];
    client_command_params: Structure[];
    response_params: Structure[];
    client_notification_params: Structure[];
    server_notification_params: Structure[];
    messages: {
        server_requests: Request[];
        server_notifications: Notification[];
        client_requests: Request[];
        client_notifications: Notification[];
    };
    enums: string[];
}

interface StructureGroup { name: string, structures: Structure[] }

function to_camelcase(text: string): string {
    return text.replace(/_[a-z]/g, text => text[1].toUpperCase());
}

function to_c_name(text: string): string {
    return text.toLowerCase().replace(/[$\s/]/, '_');
}

function is_array(text: string): boolean {
    if (typeof text === 'string')
        return text.endsWith('[]');
    else
        throw new Error("text is " + (typeof text));
}

function get_type(type_name: string): string {
    var type_is_array = is_array(type_name);
    type_name = type_name.replace('[]', ''); // strip array postfix
    var retval: string;
    switch (type_name){
        case 'string':
            retval = 'char *';
            break;
        case 'boolean':
            retval = 'bool';
            break;
        case 'number':
            retval = 'int64_t';
            break;
        case 'any':
            return 'json_t *'; // don't care about arrays here
        default:
            retval = `${type_name}_t`
            break;
    }
    if (type_is_array)
        retval += ' *'
    return retval;
}

function get_json_type(type_name: string): string {
    if (is_array(type_name))
        return 'array';
    type_name = type_name.replace('[]', ''); // strip array postfix
    var retval: string;
    switch (type_name){
        case 'string':
        case 'uri':
            return 'string';
        case 'boolean':
            return 'boolean';
        case 'number':
            return 'integer';
        case 'any':
            throw new Error("No json check should be done for the `any` type.");
        default:
            return 'object';
    }
}

function enum_field_name(structure: string, field: string): string {
    return `${to_c_name(structure).toUpperCase()}_FIELD_${field.toUpperCase()}`
}

function space_padding(elements: string[], element_name: string): string {
    return ' '.repeat(Math.max(...elements.map(e => e.length)) - element_name.length);
}

function code_separator(name: string): string {
    return ['\n/' + '*'.repeat(79), ` * ${name}`, ' ' + '*'.repeat(78) + '/\n'].join('\n');
}

function is_freeable(type: string): boolean {
    return !database.enums.find(e => e === type) && type !== 'boolean' && type !== 'number';
}

var database = JSON.parse(fs.readFileSync('backend/generator/protocol/structures.json').toString()) as Database;


var structures = [].concat(database.common,
    database.server_command_params,
    database.client_command_params,
    database.response_params,
    database.client_notification_params,
    database.server_notification_params);

var groups: StructureGroup[] = [
    {name: "Common parameter", structures: database.common},
    {name: "Server command parameter", structures: database.server_command_params},
    {name: "Client command parameter", structures: database.client_command_params},
    {name: "Command response parameter", structures: database.response_params},
    {name: "Client notification parameter", structures: database.client_notification_params},
    {name: "Server notification parameter", structures: database.server_notification_params}
];

function generate_defines(): string {
    return Object.keys(database.defines).map(key => {
        var value: string = database.defines[key];
        return `#define ${key.toUpperCase()} "${value}"`;
    }).join('\n');
}

function generate_field_enums(): string {
    return groups.map(g => {
        return `/* ${g.name} fields */\n` + g.structures.map(s => {
            return `typedef enum\n` +
            '{\n' +
            Object.keys(s.members).map((key, index) => {
                return `    ${enum_field_name(s.name, key) + space_padding(Object.keys(s.members), key)} = (1 << ${index}),\n`;
            }).join('') +
            '\n' +
            `    ${enum_field_name(s.name, 'ALL')} = (0x${((1 << Object.keys(s.members).length) - 1).toString(16)}),\n` +
            (s.required.length > 0 ? `    ${enum_field_name(s.name, 'REQUIRED')} = (${(s.required.length == Object.keys(s.members).length) ? enum_field_name(s.name, 'ALL') : s.required.map(f => enum_field_name(s.name, f)).join(' | ')})\n` : '') +
            `} ${to_c_name(s.name)}_fields_t;\n\n`;
        }).join('');
    }).join('\n\n');
}

function generate_struct_members(members: any, indent = '    '): string {
    return Object.keys(members).map(key => {
        var repr = '';
        if (typeof members[key] === 'object'){
            repr += indent + `struct\n`;
            repr += indent + `{\n`;
            repr += generate_struct_members(members[key], indent + '    ');
            repr += indent + `} ${key};\n`;
        }
        else {
            var key_is_array = is_array(members[key]);
            repr += indent + `${get_type(members[key])} ${(key_is_array ? 'p_' : '') + key};\n`;
            if (key_is_array)
                repr += indent + `uint32_t ${key}_count;\n`;
        }
        return repr;
    }).join('');
}

function generate_structs(): string {
    return groups.map(g => {
        return `/* ${g.name} structures */\n` + g.structures.map(s => {
            return `typedef struct\n` +
            '{\n' +
            `    ${s.name}_fields_t valid_fields;\n\n` +
            generate_struct_members(s.members) +
            `} ${s.name}_t;`
        }).join('\n\n');
    }).join('\n\n');
}

function generate_enum_encoders(): string {
    return database.enums.map(e => {
        return `static inline json_t * encode_${e}(${e}_t value)\n` +
        `{\n` +
        `    return json_integer(value);\n` +
        `}\n`;
    }).join('\n');
}

function generate_member_encoders(members: any, name: string, indent = '    ', json_value='p_json', field_value='value'): string {
    return Object.keys(members).map(key => {
        var retval: string[] = [];
        if (json_value === 'p_json') {
            retval.push(`if (${field_value}.valid_fields & ${enum_field_name(name, key)})`);
            retval.push(`{`);
        }

        if (typeof members[key] === 'object'){
            retval.push(`    json_t * p_${key}_json = json_object();`);
            retval = retval.concat(generate_member_encoders(members[key], `${field_value}.${key}`, indent.slice(4), `p_${key}_json`, field_value + '.' + key));
            retval.push(`    json_object_set_new(${json_value}, "${to_camelcase(key)}", p_${key}_json);`);
        }
        else if (members[key] === 'any') {
            retval.push(`    json_object_set_new(${json_value}, "${to_camelcase(key)}", ${field_value}.${key});`)
        }
        else {
            if (is_array(members[key])) {
                retval.push(`    json_t * p_${key}_json = json_array();`);
                retval.push(`    for (uint32_t i = 0; i < ${field_value}.${key}_count; ++i)`);
                retval.push(`    {`);
                retval.push(`        json_array_append_new(p_${key}_json, encode_${members[key].replace('[]', '')}(${field_value}.p_${key}[i]));`);
                retval.push(`    }`);
                retval.push(`    json_object_set_new(${json_value}, "${to_camelcase(key)}", p_${key}_json);`);
            }
            else {
                retval.push(`    json_object_set_new(${json_value}, "${to_camelcase(key)}", encode_${members[key]}(${field_value}.${key}));`);
            }
        }
        if (json_value === 'p_json') {
            retval.push(`}`);
            return retval.map(line => indent + line + '\n').join('');
        }
        else
            return retval.join('');
    }).join('\n');
}

function generate_encoder_declarations(): string {
    return groups.map(g => {
        return `/* ${g.name} JSON encoders */\n` + g.structures.map(s => {
            return `json_t * encode_${s.name}(${s.name}_t value);\n`;
        }).join('');
    }).join('');
}

function generate_encoder_definitions(): string {
    return groups.map(g => {
        return `/* ${g.name} JSON encoders */\n` + g.structures.map(s => {
            return `json_t * encode_${s.name}(${s.name}_t value)\n` +
            `{\n` +
            `    json_t * p_json = json_object();\n` +
            `\n` +
            generate_member_encoders(s.members, s.name) +
            `    return p_json;\n` +
            `}`;
        }).join('\n\n');
    }).join('\n\n');
}

function generate_enum_decoders(): string {
    return database.enums.map(e => {
        return `static inline ${e}_t decode_${e}(json_t * p_json)\n` +
        `{\n` +
        `    return (${e}_t) decode_number(p_json);\n` +
        `}\n`;
    }).join('\n');
}

function generate_member_decoders(members: any, structure_name: string, indent='    ', retval='retval'): string {
    return Object.keys(members).map(key => {
        var field_setter = indent + `    ${retval}.valid_fields |= ${enum_field_name(structure_name, key)};\n`;
        var setter = '';
        setter += indent + `json_t * p_${key}_json = json_object_get(p_json, "${to_camelcase(key)}");\n`
        if (typeof members[key] === 'object'){
            setter += indent + `if (json_is_object(p_${key}_json))\n`;
            setter += indent + `{\n`;
            setter += generate_member_decoders(members[key], structure_name, indent + '    ', retval + '.' + key);
            if (retval === 'retval')
                setter += field_setter;
            setter += indent + `}\n`;
        }
        else if (members[key] === 'any') {
            setter += indent + `if (p_${key}_json != NULL)\n`;
            setter += indent + `{\n`;
            setter += indent + `    ${retval}.${key} = json_deep_copy(p_${key}_json);\n`
            if (retval === 'retval')
                setter += field_setter;
            setter += indent + `}\n`;
        }
        else {
            setter += indent + `if (json_is_${get_json_type(members[key])}(p_${key}_json))\n`;
            setter += indent + `{\n`;
            if (is_array(members[key])) {
                setter += indent + `    ${retval}.${key}_count = json_array_size(p_${key}_json);\n`;
                setter += indent + `    ${retval}.p_${key} = malloc(sizeof(${get_type(members[key]).replace('*', '').trim()}) * ${retval}.${key}_count);\n`;
                setter += indent + `    ASSERT(${retval}.p_${key});\n`;
                setter += indent + `    json_t * p_it;\n`;
                setter += indent + `    uint32_t index;\n`;
                setter += indent + `    json_array_foreach(p_${key}_json, index, p_it)\n`;
                setter += indent + `    {\n`;
                setter += indent + `        ${retval}.p_${key}[index] = decode_${members[key].replace('[]','')}(p_it);\n`;
                setter += indent + `    }\n`;
            }
            else {
                setter += indent + `    ${retval}.${key} = decode_${members[key]}(p_${key}_json);\n`;
            }
            if (retval === 'retval')
                setter += field_setter;
            setter += indent + `}\n`
        }
        setter += '\n';

        return setter;
    }).join('');
}

function generate_decoder_declarations(): string {
    return groups.map(g => {
        return `/* ${g.name} JSON decoders */\n` + g.structures.map(s => {
            return `${s.name}_t decode_${s.name}(json_t * p_json);\n`;
        }).join('');
    }).join('');
}


function generate_decoder_definitions(): string {
    return groups.map(g => {
        return `/* ${g.name} JSON decoders */\n` + g.structures.map(s => {
            var retval: string[] = [];
            retval.push(`${s.name}_t decode_${s.name}(json_t * p_json)`)
            retval.push(`{`)
            retval.push(`    ${s.name}_t retval;`)
            retval.push(`    memset(&retval, 0, sizeof(retval));`)
            retval.push(``)
            retval.push(generate_member_decoders(s.members, s.name))
            if (s.required.length > 0) {
                retval.push(`    if ((retval.valid_fields & ${enum_field_name(s.name, "REQUIRED")}) != ${enum_field_name(s.name, "REQUIRED")})`)
                retval.push(`    {`)
                retval.push(`        m_error = DECODER_ERROR_MISSING_REQUIRED_FIELDS;`)
                retval.push(`        LOG("Missing required parameters for ${s.name}: Got 0x%x, expected 0x%x\\n", retval.valid_fields, ${enum_field_name(s.name, "REQUIRED")});`)
                retval.push(`    }`)
            }
            retval.push(`    return retval;`)
            retval.push(`}`)
            retval.push(``)
            return retval.map(line => line + '\n').join('');
        }).join('');
    }).join('\n\n');
}

function generate_decoder_source_extras(): string {
    var retval: string[] = [];
    retval.push(`decoder_error_t decoder_error(void)`);
    retval.push(`{`);
    retval.push(`    decoder_error_t e = m_error;`);
    retval.push(`    m_error = DECODER_ERROR_NONE;`);
    retval.push(`    return e;`);
    retval.push(`}`);
    return retval.map(line => line + '\n').join('');
}

function generate_decoder_header_extras(): string {
    var retval: string[] = [];
    retval.push(`typedef enum`);
    retval.push(`{`);
    retval.push(`    DECODER_ERROR_NONE,`);
    retval.push(`    DECODER_ERROR_MISSING_REQUIRED_FIELDS`);
    retval.push(`} decoder_error_t;`);
    retval.push(``);
    retval.push(`decoder_error_t decoder_error(void);`);
    return retval.map(line => line + '\n').join('');
}

function generate_member_freers(members: any, name: string, field_value='value'): string {
    return Object.keys(members).map(key => {
        var retval: string[] = [];
        if (typeof members[key] === 'object') {
            retval.push(generate_member_freers(members[key], `${field_value}.${key}`, field_value + '.' + key));
        }
        else if (members[key] === 'any') {
            retval.push(`json_decref(${field_value}.${key});`)
        }
        else {
            if (is_array(members[key])) {
                if (is_freeable(members[key].replace('[]', ''))) {
                    retval.push(`for (uint32_t i = 0; i < ${field_value}.${key}_count; ++i)`);
                    retval.push(`{`);
                    retval.push(`    free_${members[key].replace('[]', '')}(${field_value}.p_${key}[i]);`)
                    retval.push(`}`);
                }
                retval.push(`free(${field_value}.p_${key});`)
            }
            else {
                if (is_freeable(members[key].replace('[]', ''))) {
                    retval.push(`free_${members[key]}(${field_value}.${key});`)
                }
            }
        }
        return retval.map(line => '    ' + line + '\n').join('');
    }).join('').replace(/(\s+[\n\r])+/, '');
}

function generate_freer_declarations(): string {
    return groups.map(g => {
        return `/* ${g.name} structure freers */\n` + g.structures.map(s => {
            return `void free_${s.name}(${s.name}_t value);\n`;
        }).join('');
    }).join('');
}

function generate_freer_definitions(): string {
    return groups.map(g => {
        return `/* ${g.name} structure freers */\n` + g.structures.map(s => {
            return `void free_${s.name}(${s.name}_t value)\n` +
            `{\n` +
            generate_member_freers(s.members, s.name) +
            `}\n`;
        }).join('\n');
    }).join('\n\n');
}

function generate_request_handler_message_names(): string {
    return database.messages.server_requests.map(r => {
        return `#define LSP_REQUEST_${to_c_name(r.name).toUpperCase()} "${to_camelcase(r.name)}"\n`;
    }).join('');
}

function generate_notification_handler_message_names(): string {
    return database.messages.server_notifications.map(r => {
        return `#define LSP_NOTIFICATION_${to_c_name(r.name).toUpperCase()} "${to_camelcase(r.name)}"\n`;
    }).join('');
}

function generate_client_message_names(): string {
    return database.messages.client_notifications.map(r => {
        return `#define LSP_NOTIFICATION_${to_c_name(r.name).toUpperCase()} "${to_camelcase(r.name)}"\n`;
    }).join('') +
    database.messages.client_requests.map(r => {
        return `#define LSP_REQUEST_${to_c_name(r.name).toUpperCase()} "${to_camelcase(r.name)}"\n`;
    }).join('');
}

function generate_request_handler_function_types(): string {
    return database.messages.server_requests.map(r => {
        return `typedef void (*lsp_request_handler_${to_c_name(r.name)}_t)(const ${r.parameters}_t * p_params, json_t * p_response);\n`;
    }).join('');
}

function generate_notification_handler_function_types(): string {
    return database.messages.server_notifications.map(r => {
        return `typedef void (*lsp_notification_handler_${to_c_name(r.name)}_t)(const ${r.parameters}_t * p_params);\n`;
    }).join('');
}

function generate_request_handler_registrator_declaration(): string {
    return database.messages.server_requests.map(r => {
        return `void lsp_request_handler_${to_c_name(r.name)}_register(lsp_request_handler_${to_c_name(r.name)}_t handler);\n`;
    }).join('');
}

function generate_notification_handler_registrator_declaration(): string {
    return database.messages.server_notifications.map(r => {
        return `void lsp_notification_handler_${to_c_name(r.name)}_register(lsp_notification_handler_${to_c_name(r.name)}_t handler);\n`;
    }).join('');
}

function generate_request_handler_functions(): string {
    return database.messages.server_requests.map(r => {
        return `static lsp_request_handler_${to_c_name(r.name)}_t mp_request_handler_${to_c_name(r.name)};\n`;
    }).join('');
}

function generate_notification_handler_functions(): string {
    return database.messages.server_notifications.map(r => {
        return `static lsp_notification_handler_${to_c_name(r.name)}_t mp_notification_handler_${to_c_name(r.name)};\n`;
    }).join('');
}

function generate_request_handlers(): string {
    return database.messages.server_requests.map(r => {
        var retval: string[] = [];
        retval.push(`static void request_handler_${to_c_name(r.name)}(const char * p_method, json_t * p_params, json_t * p_response)`);
        retval.push(`{`);
        retval.push(`    ${r.parameters}_t params = decode_${r.parameters}(p_params);`);
        retval.push(`    if (decoder_error() == DECODER_ERROR_NONE)`);
        retval.push(`    {`);
        retval.push(`        mp_request_handler_${to_c_name(r.name)}(&params, p_response);`);
        retval.push(`    }`);
        retval.push(`    else`);
        retval.push(`    {`);
        retval.push(`        json_rpc_error_response_send(p_response, JSON_RPC_ERROR_INVALID_PARAMS, "Required parameters missing", NULL);`);
        retval.push(`    }`);
        retval.push(`    free_${r.parameters}(params);`);
        retval.push(`}`);
        return retval.map(line => line + '\n').join('');
    }).join('');
}

function generate_notification_handlers(): string {
    return database.messages.server_notifications.map(r => {
        var retval: string[] = [];
        retval.push(`static void notification_handler_${to_c_name(r.name)}(const char * p_method, json_t * p_params)`);
        retval.push(`{`);
        retval.push(`    ${r.parameters}_t params = decode_${r.parameters}(p_params);`);
        retval.push(`    mp_notification_handler_${to_c_name(r.name)}(&params);`);
        retval.push(`    free_${r.parameters}(params);`);
        retval.push(`}`);
        return retval.map(line => line + '\n').join('');
    }).join('');
}

function generate_request_handler_registrator_definition(): string {
    return database.messages.server_requests.map(r => {
        var retval: string[] = [];
        retval.push(`void lsp_request_handler_${to_c_name(r.name)}_register(lsp_request_handler_${to_c_name(r.name)}_t handler)`);
        retval.push(`{`);
        retval.push(`    mp_request_handler_${to_c_name(r.name)} = handler;`)
        retval.push(`    json_rpc_request_handler_add(LSP_REQUEST_${to_c_name(r.name).toUpperCase()}, request_handler_${to_c_name(r.name)});`)
        retval.push(`}`)
        return retval.map(line => line + '\n').join('');
    }).join('');
}

function generate_notification_handler_registrator_definition(): string {
    return database.messages.server_notifications.map(r => {
        var retval: string[] = [];
        retval.push(`void lsp_notification_handler_${to_c_name(r.name)}_register(lsp_notification_handler_${to_c_name(r.name)}_t handler)`);
        retval.push(`{`);
        retval.push(`    mp_notification_handler_${to_c_name(r.name)} = handler;`)
        retval.push(`    json_rpc_notification_handler_add(LSP_NOTIFICATION_${to_c_name(r.name).toUpperCase()}, notification_handler_${to_c_name(r.name)});`)
        retval.push(`}`)
        return retval.map(line => line + '\n').join('');
    }).join('');
}

var header =
`#include <jansson.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "native.h"
#include "uri.h"
`

fs.writeFile('backend/include/protocol/structures.h',
    '#pragma once\n' +
    header +
    code_separator('Defines') +
    generate_defines() +
    code_separator('Field enumerators') +
    generate_field_enums() +
    code_separator('Parameter structures') +
    generate_structs());

header += '#include "structures.h"\n\n';

fs.writeFile('backend/include/protocol/encoders.h',
    '#pragma once\n' +
    header +
    generate_enum_encoders() +
    generate_encoder_declarations());

fs.writeFile('backend/src/protocol/encoders.c',
    '#include "encoders.h"\n' +
    header +
    generate_encoder_definitions());

fs.writeFile('backend/include/protocol/decoders.h',
    '#pragma once\n' +
    header +
    generate_decoder_header_extras() +
    code_separator('Decoders') +
    generate_enum_decoders() +
    generate_decoder_declarations() +
    code_separator('Freers') +
    generate_freer_declarations());

fs.writeFile('backend/src/protocol/decoders.c',
    header +
    '#include "decoders.h"\n' +
    '#include "log.h"\n' +
    '\nstatic decoder_error_t m_error;\n' +
    code_separator('Decoders') +
    generate_decoder_definitions() +
    code_separator('Freers') +
    generate_freer_definitions() +
    '\n' +
    generate_decoder_source_extras());

fs.writeFile('backend/include/protocol/message_handling.h',
    '#pragma once\n' +
    header +
    code_separator('Outgoing messages') +
    generate_client_message_names() +
    code_separator('Request handler types') +
    generate_request_handler_function_types() +
    code_separator('Notification handler types') +
    generate_notification_handler_function_types() +
    code_separator('Register functions') +
    generate_request_handler_registrator_declaration() +
    generate_notification_handler_registrator_declaration()
);


header += '#include "encoders.h"\n';
header += '#include "decoders.h"\n';
header += '#include "json_rpc.h"\n';
header += '#include "log.h"\n';
header += '#include "message_handling.h"\n';


fs.writeFile('backend/src/protocol/message_handling.c',
    header +
    code_separator('Message names') +
    generate_request_handler_message_names() +
    generate_notification_handler_message_names() +
    code_separator('Handler function pointers') +
    generate_request_handler_functions() +
    generate_notification_handler_functions() +
    code_separator('Handler functions') +
    generate_request_handlers() +
    generate_notification_handlers() +
    code_separator('Register functions') +
    generate_request_handler_registrator_definition() +
    generate_notification_handler_registrator_definition()
);

console.log("Done.");