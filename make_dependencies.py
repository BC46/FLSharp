import os
import subprocess
from pathlib import Path

def get_file_dependencies(file_path):
    result = []

    cmd = ['cl', '/nologo', '/Zs', '-Iinclude', '/showIncludes', file_path]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True)

    for line in proc.stdout:
        line = line.strip()

        if not line.startswith("Note: including file:"):
            continue

        stripped_line = line.removeprefix("Note: including file:").lstrip()

        abs_path = os.path.join(os.getcwd(), stripped_line)

        if (Path(os.getcwd()) in Path(abs_path).parents):
            result.append(abs_path)

    return result

def run():
    deps_file = 'makefile.deps'
    src_dir = 'src'
    obj_dir = 'obj'

    f = open(deps_file, "w")

    for file_name in os.listdir(src_dir):
        deps = get_file_dependencies(os.path.join(src_dir, file_name))

        obj_file = os.path.join(obj_dir, Path(file_name).stem + ".obj")
        cpp_file = os.path.join(src_dir, file_name)

        f.write(obj_file + ": \\\n")

        #f.write((" " * 4) + cpp_file + " \\\n")
        for dep in deps:
            f.write((" " * 4) + os.path.relpath(dep) + " \\\n")
        f.write((" " * 4) + "makefile\n\n")

    f.close()


if __name__ == "__main__":
    run()
