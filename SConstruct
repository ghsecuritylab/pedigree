####################################
# SCons build system for Pedigree
## Tyler Kennedy (AKA Linuxhq AKA TkTech)
## ToDO:
## ! Make the script suck less
## ! Add a check for -fno-stack-protector
## ! Make a flexible system for build defines
####################################
EnsureSConsVersion(0,98,0)

####################################
# Import various python libraries
## Do not use anything non-std
####################################
import os
import commands
import re
import string
####################################
# Add an option here to have it defined
####################################
defines = [
    'THREADS',                  # Enable threading
    'DEBUGGER',                 # Enable the debugger
    'DEBUGGER_QWERTY',          # Enable the QWERTY keymap
    #'SMBIOS',                  # Enable SMBIOS
    'SERIAL_IS_FILE',           # Don't treat the serial port like a VT100 terminal
    'ECHO_CONSOLE_TO_SERIAL',   # Puts the kernel's log over the serial port, (qemu -serial file:serial.txt or /dev/pts/0)
    'KERNEL_NEEDS_ADDRESS_SPACE_SWITCH',
    'ADDITIONAL_CHECKS',
    'BITS_32',
    'KERNEL_STANDALONE',
    'VERBOSE_LINKER',           # Increases the verbosity of messages from the Elf and KernelElf classes
]

####################################
# Default build flags (Also used to auto-generate help)
####################################
opts = Variables('options.cache')
#^-- Load saved settings (if they exist)
opts.AddVariables(
    ('CROSS','Base for cross-compilers, tool names will be appended automatically',''),
    ('CC','Sets the C compiler to use.'),
    ('CXX','Sets the C++ compiler to use.'),
    ('AS','Sets the assembler to use.'),
    ('LINK','Sets the linker to use.'),
    ('CFLAGS','Sets the C compiler flags.','-march=i486 -fno-builtin -nostdlib -m32 -g0 -O3'),
    ('CXXFLAGS','Sets the C++ compiler flags.','-march=i486 -fno-builtin -nostdlib -m32 -g0 -O3 -Weffc++ -Wold-style-cast -Wno-long-long -fno-rtti -fno-exceptions'),
    ('ASFLAGS','Sets the assembler flags.','-f elf'),
    ('LINKFLAGS','Sets the linker flags','-T src/system/kernel/core/processor/x86/kernel.ld -nostdlib -nostdinc'),
    ('BUILDDIR','Directory to place build files in.','build'),
    ('LIBGCC','The folder containing libgcc.a.',''),
    BoolVariable('verbose','Display verbose build output.',0),
    BoolVariable('verbose_link','Display verbose linker output.',0),
    BoolVariable('warnings', 'compilation with -Wall and similiar', 1),
	BoolVariable('installer', 'Build the installer', 0)
)

env = Environment(options = opts,tools = ['default'],ENV = os.environ)
#^-- Create a new environment object passing the options
Help(opts.GenerateHelpText(env))
#^-- Create the scons help text from the options we specified earlier
opts.Save('options.cache',env)
#^-- Save the cache file over the old one

# Pedigree is to be built on a POSIXy host (Cygwin on Windows)
env.Platform('posix')

# Trickery to get around the case-insensitivity on Windows
env['OBJSUFFIX'] = ".obj"

# Explicitly defined to get around SCons picking this up from the environment
env['PROGSUFFIX'] = ''

# Pedigree binary locations
env['PEDIGREE_BUILD_BASE'] = env['BUILDDIR']
env['PEDIGREE_BUILD_MODULES'] = env['BUILDDIR'] + '/modules'
env['PEDIGREE_BUILD_KERNEL'] = env['BUILDDIR'] + '/kernel'
env['PEDIGREE_BUILD_DRIVERS'] = env['BUILDDIR'] + '/drivers'
env['PEDIGREE_BUILD_SUBSYS'] = env['BUILDDIR'] + '/subsystems'
env['PEDIGREE_BUILD_APPS'] = env['BUILDDIR'] + '/apps'

# Set the compilers if CROSS is not an empty string
if env['CROSS'] != '':
    crossBase = env['CROSS']
    env['CC'] = crossBase + 'gcc'
    env['CXX'] = crossBase + 'g++'
    env['LD'] = crossBase + 'gcc'
    env['AS'] = crossBase + 'as'
    env['AR'] = crossBase + 'ar'

####################################
# Compiler/Target specific settings
####################################
# Check to see if the compiler supports --fno-stack-protector
out = commands.getoutput('echo -e "int main(void) {return 0;}" | ' + env['CC'] + ' -x c -fno-stack-protector -c -o /dev/null -')
if not 'unrecognized option' in out:
    env['CXXFLAGS'] += ' -fno-stack-protector'
    env['CFLAGS'] += ' -fno-stack-protector'

out = commands.getoutput(env['CXX'] + ' -v')
#^-- The old script used --dumpmachine, which isn't always present
tmp = re.match('.*?Target: ([^\n]+)',out,re.S)

if tmp != None:
    env['ARCH_TARGET'] = tmp.group(1)

    if re.match('i[3456]86',tmp.group(1)) != None:
        defines +=  ['X86','X86_COMMON','LITTLE_ENDIAN']
        #^-- Should provide overloads for these...like machine=ARM_VOLITILE
        
        env['ARCH_TARGET'] = 'X86'
    elif re.match('amd64|x86[_-]64',tmp.group(1)) != None:
        defines += ['X64']
        
        env['ARCH_TARGET'] = 'X64'
    elif re.match('ppc|powerpc',tmp.group(1)) != None:
        defines += ['PPC']
        
        env['ARCH_TARGET'] = 'PPC'
    elif re.match('arm',tmp.group(1)) != None:
        defines += ['ARM']
        
        env['ARCH_TARGET'] = 'ARM'

# Default to x86 if something went wrong
else:
    env['ARCH_TARGET'] = 'X86'
        
# LIBGCC path
if env['LIBGCC'] == '':
    crossPath = os.path.dirname(env['CC'])
    
    libgccPath = commands.getoutput(env['CXX'] + ' --print-libgcc-file-name')
    if not os.path.exists(libgccPath):
        print "Error: libgcc path could not be determined. Use the LIBGCC option."
        Exit(1)
    
    env['LIBGCC'] = os.path.dirname(libgccPath)

# NASM is used for X86 and X64 builds
if env['ARCH_TARGET'] == 'X86' or env['ARCH_TARGET'] == 'X64':
    crossPath = os.path.dirname(env['CC'])
    env['AS'] = crossPath + '/nasm'

####################################
# Some quirks
####################################
defines += ['__UD_STANDALONE__']
#^-- Required no matter what.

if env['warnings']:
    env['CXXFLAGS'] += ' -Wall'

if env['verbose_link']:
    env['LINKFLAGS'] += ' --verbose'

if env['installer']:
	defines += ['INSTALLER']

####################################
# Fluff up our build messages
####################################
if not env['verbose']:
    env['CCCOMSTR']   =    '     Compiling \033[32m$TARGET\033[0m'
    env['CXXCOMSTR']  =    '     Compiling \033[32m$TARGET\033[0m'
    env['ASCOMSTR']   =    '    Assembling \033[32m$TARGET\033[0m'
    env['LINKCOMSTR'] =    '       Linking \033[32m$TARGET\033[0m'
    env['ARCOMSTR']   =    '     Archiving \033[32m$TARGET\033[0m'
    env['RANLIBCOMSTR'] =  '      Indexing \033[32m$TARGET\033[0m'
    env['NMCOMSTR']   =    '  Creating map \033[32m$TARGET\033[0m'
    env['DOCCOMSTR']  =    '   Documenting \033[32m$TARGET\033[0m'

####################################
# Setup our build options
####################################
env['CPPDEFINES'] = []
env['CPPDEFINES'] = [i for i in defines]
#^-- Stupid, I know, but again I plan on adding some preprocessing (AKA auto-options for architecutres)

####################################
# Generate Version.cc
# Exports:
## PEDIGREE_BUILDTIME
## PEDIGREE_REVISION
## PEDIGREE_FLAGS
## PEDIGREE_USER
## PEDIGREE_MACHINE
####################################
if os.path.exists('/bin/date'):
    out = commands.getoutput('/bin/date \"+%k:%M %A %e-%b-%Y\"')
    tmp = re.match('^[^\n]+',out)
    env['PEDIGREE_BUILDTIME'] = tmp.group()
else:
    env['PEDIGREE_BUILDTIME'] = '(Unknown)'

if os.path.exists(commands.getoutput("which git")):
    out = commands.getoutput(commands.getoutput("which git") + ' rev-parse --verify HEAD --short')
    env['PEDIGREE_REVISION'] = out
else:
    env['PEDIGREE_REVISION'] = '(Unknown)'
    
if os.path.exists(commands.getoutput("which whoami")):
    out = commands.getoutput(commands.getoutput("which whoami"))
    tmp = re.match('^[^\n]+',out)
    env['PEDIGREE_USER'] = tmp.group()
else:
    env['PEDIGREE_USER'] = '(Unknown)'
    
if os.path.exists(commands.getoutput("which uname")):
    out = commands.getoutput(commands.getoutput("which uname"))
    tmp = re.match('^[^\n]+',out)
    env['PEDIGREE_MACHINE'] = tmp.group()
else:
    env['PEDIGREE_MACHINE'] = '(Unknown)'
    
env['PEDIGREE_FLAGS'] = string.join(env['CPPDEFINES'],' ')
# Write the file to disk (We *assume* src/system/kernel/)
file = open('src/system/kernel/Version.cc','w')
file.writelines(['const char *g_pBuildTime = "',    env['PEDIGREE_BUILDTIME'],'";\n'])
file.writelines(['const char *g_pBuildRevision = "',env['PEDIGREE_REVISION'],'";\n'])
file.writelines(['const char *g_pBuildFlags = "',   env['PEDIGREE_FLAGS'],'";\n'])
file.writelines(['const char *g_pBuildUser = "',    env['PEDIGREE_USER'],'";\n'])
file.writelines(['const char *g_pBuildMachine = "', env['PEDIGREE_MACHINE'],'";\n'])
file.writelines(['const char *g_pBuildTarget = "',  env['ARCH_TARGET'],'";\n'])
file.close()

####################################
# Progress through all our sub-directories
####################################
SConscript('SConscript', exports = ['env'], build_dir = env['BUILDDIR'], duplicate = 0)
