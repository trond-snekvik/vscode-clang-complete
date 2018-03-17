import * as cp from 'child_process';
import * as rpc from 'vscode-jsonrpc';
import * as vscode from 'vscode-languageserver-types';
import * as fs from 'fs';


let childProcess = cp.spawn('../backend/build/Debug/backend.exe');

let logger = <rpc.Logger> {
    error: (message: string) => fs.appendFile(`log.error.txt`, message),
    log: (message: string) => fs.appendFile(`log.log.txt`, message),
    info: (message: string) => fs.appendFile(`log.info.txt`, message),
    warn: (message: string) => fs.appendFile(`log.warn.txt`, message),
}

var reader = new rpc.StreamMessageReader(childProcess.stdout);
// Use stdin and stdout for communication:
let connection = rpc.createMessageConnection(
    reader,
    new rpc.StreamMessageWriter(childProcess.stdin),
    logger);

let set_file_request = new rpc.RequestType<any, any, string, any>('set_file');
let complete_at_request = new rpc.RequestType<any, any, string, any>('complete_at');
let exit_request = new rpc.RequestType0<any, string, any>('exit');

reader.onPartialMessage(e => {
    logger.info("partial!\n");
});
connection.listen();

connection.sendRequest(set_file_request, {
        filename: "../backend/test/target_files/test.c",
        includes: [],
        defines: []
    }).then((rsp) => {
        console.dir('RSP: ' + rsp);
    }, reason => console.log(reason));
connection.sendRequest(complete_at_request, {
        line: 51,
        column: 7
    }).then((rsp) => {
        console.dir('RSP: ' + rsp);
    }, reason => console.log(reason));

connection.sendRequest(exit_request);