import sys
import subprocess
import json
import time
import threading

assert(len(sys.argv) == 4)

HEADER = 'Content-Length: '

backend = sys.argv[1]
input_file = sys.argv[2]
output_file = sys.argv[3]

commands = []

with open(input_file) as f:
    commands = [line.strip() for line in f]


def getMessages(stream):
    messages = []
    header = str(stream.readline(), 'utf-8')
    while header:
        if header.startswith(HEADER):
            length = int(header[len(HEADER):])
            stream.readline() # fetch separating line
            msg = str(stream.read(length), 'utf-8')
            messages.append(msg)
            with open(output_file, 'a') as f:
                f.write(msg + '\n')
        else:
            raise Exception("Invalid packet format: " + header)
        header = str(stream.readline(), 'utf-8')
    return messages

encoded_input = input_file + '.encoded'
with open(encoded_input, 'w') as f:
    for cmd in commands:
        f.write(HEADER + str(len(cmd)) + '\r\n\r\n' + cmd)

process = subprocess.Popen(args=[backend, encoded_input], executable=backend, stdout=subprocess.PIPE)
input_thread = threading.Thread(target=getMessages, name='input thread', args=(process.stdout, ))
try:
    input_thread.start()
    # command_packets = []

    for command in range(len(commands)):
        # print('Sending command ' + json.loads(command)['method'])
        # command_packet = HEADER + str(len(command)) + '\r\n\r\n' + command
        # command_packets.append(command_packet)
        # process.stdin.write(command_packet)
        time.sleep(1)
        sys.stdout.write('[' + str(command+1) + '/' + str(len(commands)) + '] ' + format((((command+1) / len(commands)) * 100.0), '.2f') + '%\n')
finally:
    process.terminate()
