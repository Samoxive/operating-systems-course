import random
import sys
import subprocess
import os

colors = [
    'red',
    'green',
    'blue',
    'orange',
    'yellow',
    'white',
    'black',
    'purple'
]

exec_location = 'build'
executables = [
    '/psearch_normal',
    '/psearch1a',
    '/psearch1b',
    '/psearch1c',
    '/psearch2a',
    '/psearch2b',
    '/psearch2c',
    '/psearch2d',
    '/psearch3'
]
executables = ['{}{}'.format(exec_location, executable)
               for executable in executables]

input_count = int(sys.argv[1])
skip_file_creation = False

file_names = ["input{}.txt".format(i + 1) for i in range(input_count)]
if not skip_file_creation:
    for file_name in file_names:
        with open(file_name, "w") as f:
            for _ in range(5000000):
                f.write("{}, ".format(random.choice(colors)))
            f.write("{}".format(random.choice(colors)))

hyperfine_spawn = "hyperfine --prepare 'sync; rm -f output.txt' --show-output --export-csv {}.csv --export-markdown {}.md ".format(input_count, input_count) + \
    ' '.join(["'{} orange {} {} output.txt'".format(
        executable, input_count, ' '.join(file_names)) for executable in executables])
hyperfine_proc = os.system(hyperfine_spawn)
