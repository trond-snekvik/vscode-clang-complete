import * as fs from 'fs';

var snippets: string[] = [];

var data = fs.readFileSync('../../backend/generator/protocol/protocol.md');
var str = data.toString();
var reg = new RegExp(/```typescript([\s\S]+)```/g)
var match: RegExpExecArray;
while (match = reg.exec(str)) {
    snippets.push(match[1]);
}



class Request {
    constructor(public method: string, public paramName?: string){}
}

var requests: Request[] = [];

var request = new RegExp(/_Request_:[\s\S]+?method: '(\w+?)'[\s\S]+?params: (?:`(\w+?)`|void)/g)
while (match = request.exec(str)) {
    requests.push(new Request(match[1], match[2]));
}

class Member {
    constructor(public name: string, public type: string, public optional: boolean) {}
}

class Structure extends Member {
    extends?: Structure[];
    members: Member[];
    constructor(name, optional){
        super(name, 'Object', optional)
        this.members = [];
    }
}

var typescript = snippets.join('\n');
typescript = typescript.replace(/\/\*[\s\S]*?\*\//, '').replace(/\/\/.*/, '');

function parseParams(text): Member[] {
    var paramRegex = new RegExp(/^\s*(\w+\??): ({|.*?;)$/gm);
    var members: Member[] = [];

    var match;
    while (match = paramRegex.exec(text)) {
        if (match[2] === '{') {

        }
    }
    return members;
}

var interfaceRegEx = new RegExp(/interface\s+(\w+)\s+(?:extends\s+(\w+))\s*{([\s\S]+?)^}/gm);
while (match = interfaceRegEx.exec(typescript)) {
    var paramRegex = new RegExp(/^\s*(\w+): (.*?);$/gm);
}