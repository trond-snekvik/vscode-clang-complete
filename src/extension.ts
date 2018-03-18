'use strict';
import * as vscode from 'vscode';
import * as client from 'vscode-languageclient';
import * as jsonrpc from 'vscode-jsonrpc';
import * as fs from 'fs';
import * as path from 'path';

interface InitializationOptions {
    flags: string[];
    compilationDatabase: string[];
}

var langClient: client.LanguageClient;
var CONFIG_SECTION = 'clang-server';

var ARGS_FILES = [
    ".clang_complete",
    ".clang-complete"
]

function server_launch() {
    var config = vscode.workspace.getConfiguration();
    console.log("CONFIG: " + config);
    var flags = <string[]> config.get(CONFIG_SECTION + '.flags');
    var db = config.get(CONFIG_SECTION + '.compile_commands');
    var initOptions = <InitializationOptions>{ flags: flags, compilationDatabase: db };
    console.log("INIT OPTIONS: " + initOptions);
    console.log("FLAGS: " + flags);
    console.log("DB: " + db);

    // look for various clang-complete arguments files:
    if (!flags) {
        for (var file in ARGS_FILES) {
            if (fs.existsSync(file)) {
                flags = fs.readFileSync(file).toString().split(/[\n\r]/);
                break;
            }
        }
    }

    var serverOptions: client.ServerOptions = {command: __dirname + '/../../backend/bin/backend.exe', options: {env: {"LIBCLANG_NOTHREADS": 1}}};
    var clientOptions: client.LanguageClientOptions = {
        documentSelector: [{language: 'c'}, {language: 'cpp'}],
        diagnosticCollectionName: 'clang_diags',
        initializationOptions: initOptions};
    langClient = new client.LanguageClient('clang', serverOptions, clientOptions);
    langClient.start();
}

async function switch_header_and_source() {
    var filename = vscode.window.activeTextEditor.document.fileName;
    var is_header = path.extname(filename).match(/\.(h|hpp|hh)$/);
    var extensions;
    if (is_header)
        extensions = '[c|cpp|cc|cxx]';
    else
        extensions = '[h|hpp|hh]';
    var matching_filename = '**/' + path.basename(filename, path.extname(filename)) + '.' + extensions;

    var found = false;
    var finder = (uris: vscode.Uri[]) => {
        if (uris.length > 0) {
            vscode.window.showTextDocument(uris.sort((a, b) => (a.path.localeCompare(filename) - b.path.localeCompare(filename)))[0]);
            found = true;
        }
    };

    var dir = path.dirname(filename)
    do {
        await vscode.workspace.findFiles(new vscode.RelativePattern(dir, matching_filename)).then(finder);
        dir = path.dirname(dir);
    } while (!found && dir.length > 3)
}

export function activate(context: vscode.ExtensionContext) {
    server_launch();
    langClient.outputChannel.show(false);
    langClient.onReady().then(()=>{
        console.log("Ready.");
    });

    context.subscriptions.push(vscode.commands.registerCommand('clang-server.switch_header', switch_header_and_source));
    context.subscriptions.push(vscode.commands.registerCommand('clang-server.fixit', (string: string, range: vscode.Range, filename: string) => {
        console.log("Applying fix!");
        console.log(string);
        var edit = new vscode.WorkspaceEdit();
        edit.replace(vscode.Uri.file(filename), range, string);
        vscode.workspace.applyEdit(edit);
    }));
    vscode.workspace.onDidChangeConfiguration(event => {
        if (event.affectsConfiguration(CONFIG_SECTION)) {
            if (langClient) {
                langClient.stop();
                server_launch();
            }
        }
    })
}

export function deactivate() {
}