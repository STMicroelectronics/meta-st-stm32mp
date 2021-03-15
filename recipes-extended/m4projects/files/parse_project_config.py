import os
import re
import sys
import xml.etree.ElementTree as ET
from sys import argv as arg

#
# the goal is to parse project configs files to get
#    list of files to compile
#    cflags
#    ldflags

#
# convert path and checks that path is valid
#
def fullpath(filename):
    # workaround: there is a mistake in some projects
    p=filename.replace("STM32_USB_HOST_Library","STM32_USB_Host_Library")
    # some path contain windows style
    p=p.replace("\\","/")
    # contains space at the end
    p=p.replace(" ","")
    # is enclosed in double quotes
    p=filename.replace("\"","")

    # get absolute path
    p=os.path.abspath(p);

    # check if path is valid
    #print("check path: "+p)
    if os.path.exists(p)!=True:
        print("prj: "+prj)
        print("original path: "+filename)
        sys.stderr.write("error check path: "+p+" fails\n")
        exit(1);
    return p


print("start")

# arg1: path of the project
prj=arg[1]
# arg2: name of the build configuration
buildconfig=arg[2]
# arg3: root of the compilation
myroot=arg[3]

confdir=myroot+"/out/"+buildconfig+"/conf"

print("prj: "+prj)
print("myroot: "+myroot)
print("confdir: "+confdir)
print("buildconfig: "+buildconfig)

os.chdir(prj)

proj_tree = ET.parse(".project")
cproj_tree = ET.parse(".cproject")

if os.path.exists(confdir)!=True:
    os.mkdir(confdir)

if prj.find("(")!=-1:
    print("bad prj path")
    sys.stderr.write("bad prj path: "+prj+"\n")
    exit(1)

#
# get the source code file list
#
print("file list")

f=open(confdir+"/config.in", 'w')

root = proj_tree.getroot()

f.write("CSRC += \\\n")

for i in root.iter('link'):
    a=i.find('locationURI')
    if a==None:
        a=i.find('location')
    if a==None:
        print("could not find any file")
        exit(1)

    if a.text is None:
        print("no text")
    else:
        #print("text:"+a.text)
        temp=a.text

        if ((temp.find(".txt")==-1) & (temp.find(".gdb")==-1) & (temp.find(".launch")==-1) & (temp.find(".sh")==-1) & (temp.find("README")==-1)):

            # Format locationURI value
            if re.search(r'\$\%7BPARENT-.-PROJECT_LOC\%7D', temp):
                temp = re.sub('\$\%7BPARENT-(.)-PROJECT_LOC\%7D', r'PARENT-\1-PROJECT_LOC', temp)
            elif re.search(r'\$\%7BPROJECT_LOC\%7D', temp):
                temp = re.sub('\$\%7BPROJECT_LOC\%7D', r'PROJECT_LOC', temp)

            temp=temp.replace("PARENT-0-PROJECT_LOC/", "./")
            temp=temp.replace("PARENT-1-PROJECT_LOC/", "../")
            temp=temp.replace("PARENT-2-PROJECT_LOC/", "../../")
            temp=temp.replace("PARENT-3-PROJECT_LOC/", "../../../")
            temp=temp.replace("PARENT-4-PROJECT_LOC/", "../../../../")
            temp=temp.replace("PARENT-5-PROJECT_LOC/", "../../../../../")
            temp=temp.replace("PARENT-6-PROJECT_LOC/", "../../../../../../")
            temp=temp.replace("PARENT-7-PROJECT_LOC/", "../../../../../../../")
            temp=temp.replace("PROJECT_LOC/", "../")
            #print(temp)
            temp=fullpath(temp)

            f.write(temp+" \\\n")

f.write("\n")

cflags=""
ldlibs=""
ldscript=""

root = cproj_tree.getroot()

count=0
for j in root.iter('configuration'):
    temp=j.get('name')
    if temp == buildconfig:
        for i in j.iter('option'):
            a=i.get('superClass')
            if a == 'com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.includepaths':
                for j in i.iter('listOptionValue'):
                    temp=j.get('value')
                    if temp != "":
                        temp=temp.replace("\\","/")
                        # New workaround to override value when configured with ${workspace_loc:/${ProjName}/xxx}
                        temp = re.sub('\$\{[^:]*:/\$\{[^\}]*\}([^\}]*)\}', r'..\1', temp)
                        #workaround remove first occurence of "../"
                        temp=temp.replace("../", "",1)
                        temp=fullpath(temp)
                        #print(temp)
                        cflags=cflags+" -I"+temp+" \\\n"

            if a == 'com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.definedsymbols':
                for j in i.iter('listOptionValue'):
                    temp=j.get('value')
                    if temp != "":
                        #print(temp)
                        cflags=cflags+" '-D"+temp+"' \\\n"

            if a == 'com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.option.script':
                temp=i.get('value')
                # New workaround to override value when configured with ${workspace_loc:/${ProjName}/xxx}
                temp = re.sub('\$\{[^:]*:/\$\{[^\}]*\}([^\}]*)\}', r'..\1', temp)
                temp=temp.replace("../", "",1)
                temp=fullpath(temp)
                #print(temp)
                ldscript=temp

            if a == 'com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.option.directories':
                for j in i.iter('listOptionValue'):
                    temp=j.get('value')
                    # New workaround to override value when configured with ${workspace_loc:/${ProjName}/xxx}
                    temp = re.sub('\$\{[^:]*:/\$\{[^\}]*\}([^\}]*)\}', r'..\1', temp)
                    temp=temp.replace("../", "",1)
                    temp=fullpath(temp)
                    ldlibs=ldlibs+" -L"+temp+"/"

            if a == 'com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.option.libraries':
                for j in i.iter('listOptionValue'):
                    temp=j.get('value')
                    ldlibs=ldlibs+" -l"+temp

print("cflags="+cflags)
f.write("CFLAGS += "+cflags+"\n")
f.write("\n")

print("ldlibs="+ldlibs)
f.write("LDLIBS += "+ldlibs+"\n")
f.write("\n")

f.write("LDSCRIPT += "+ldscript+"\n")
f.write("\n")

f.close();

print("exit")
