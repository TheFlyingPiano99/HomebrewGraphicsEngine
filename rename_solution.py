import os
import sys
import fileinput

def delete_file(path :str):
    if (os.path.exists(path)):
        os.remove(path)

def rename_file_and_content(dir :str, extension :str, old_name :str, new_name :str):
    if (os.path.exists(dir + old_name + extension)):
        os.rename(dir + old_name + extension, dir + new_name + extension)   # rename file
    if (os.path.exists(dir + new_name + extension)):
        f = open(dir + new_name + extension,'r')
        filedata = f.read()
        f.close()
        newdata = filedata.replace(old_name, new_name)
        f = open(dir + new_name + extension,'w')
        f.write(newdata)
        f.close()


def main (new_name : str):
    os.chdir(os.path.dirname(os.path.realpath(__file__)))

    old_name = None
    for file in os.listdir():
        if (file.endswith(".sln")):
            old_name = file.replace(".sln", "")

    if (old_name):
        print("Old solution name: " + old_name)
        print("Renaming solution to: " + new_name)

        rename_file_and_content("", ".sln", old_name, new_name)
        rename_file_and_content("Sources\\", ".vcxproj", old_name, new_name)
        rename_file_and_content("Sources\\", ".vcxproj.filters", old_name, new_name)
        rename_file_and_content("Sources\\", ".rc", old_name, new_name)
        rename_file_and_content("Sources\\", ".vcxproj.user", old_name, new_name)
        rename_file_and_content("Sources\\", ".vcxproj.orig", old_name, new_name)
        rename_file_and_content("Sources\\", ".vcxproj.filters.orig", old_name, new_name)

        delete_file("x64\\Release\\" + old_name + ".exe")
        delete_file("x64\\Release\\" + old_name + ".pdb")
        delete_file("x64\\Debug\\" + old_name + ".exe")
        delete_file("x64\\Debug\\" + old_name + ".pdb")
        delete_file("Sources\\" + old_name + ".aps")

        print("Solution renamed successfully.")

    else:
        print("Solution file not found")



if __name__ == "__main__":  
    if (len(sys.argv) > 1 and len(sys.argv[1]) > 0):
        main(sys.argv[1])
    else:
        print("ERROR: Provide argument for new name!")
