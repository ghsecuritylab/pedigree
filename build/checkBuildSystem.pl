#!/usr/bin/perl
#######################################################################
# checkBuildSystem.pl - a script to check for and possibly install
#                       gcc/binutils in the build/ directory.

my $COMPILER_VERSION = "4.2.2";
my $BINUTILS_VERSION = "2.18";
my $FTP_DIR = "ftp://ftp.gnu.org/gnu/gcc/gcc-$COMPILER_VERSION";
my $BINUTILS_FTP_DIR = "ftp://ftp.gnu.org/gnu/binutils";
my $delete_cache = 0;

# Is the compiler with the given version string (e.g. i686-elf) installed?
sub is_installed {
  my ($arch) = @_;

  return -d "./compilers/$arch";
 }

# Do we have a compiler download cache?
sub is_cached {
  return 1 if (-d "./compilers/dl_cache" and
               -f "./compilers/dl_cache/gcc-core-$COMPILER_VERSION.tar.bz2" and
               -f "./compilers/dl_cache/gcc-g++-$COMPILER_VERSION.tar.bz2" and
               -f "./compilers/dl_cache/binutils-$BINUTILS_VERSION.tar.bz2");
  return 0;
}

# Download the compiler.
sub download {
  `mkdir ./compilers/dl_cache` unless (-d "./compilers/dl_cache");

  my $already_cached = 0;

  if (-f "./compilers/dl_cache/gcc-core-$COMPILER_VERSION.tar.bz2") {
    print "\e[32mGcc core files already downloaded, using cached version.\e[0m\n";
    $already_cached = 1;
  } else {
    print "\e[32mDownloading gcc core files, version $COMPILER_VERSION...\e[0m\n";  
    `cd ./compilers/dl_cache; wget $FTP_DIR/gcc-core-$COMPILER_VERSION.tar.bz2`;
    if ($? != 0) {
      print "\e[31mError: gcc core files failed to download!\e[0m\n";
      return 1;
    }
  }

  if (-f "./compilers/dl_cache/gcc-g++-$COMPILER_VERSION.tar.bz2") { 
    print "\e[32mG++ files already downloaded, using cached version.\e[0m\n";
    $already_cached = 1;
  } else {
    print "\e[32mDownloading g++, version $COMPILER_VERSION...\e[0m\n";
    `cd ./compilers/dl_cache; wget $FTP_DIR/gcc-g++-$COMPILER_VERSION.tar.bz2`;
    if ($? != 0) {
      print "\e[31mError: g++ files failed to download!\e[0m\n";
      return 1;
    }
  }

  if (-f "./compilers/dl_cache/binutils-$BINUTILS_VERSION.tar.bz2") { 
    print "\e[32mBinutils files already downloaded, using cached version.\e[0m\n";
    $already_cached = 1;
  } else {
    print "\e[32mDownloading binutils, version $BINUTILS_VERSION...\e[0m\n";
    `cd ./compilers/dl_cache; wget $BINUTILS_FTP_DIR/binutils-$BINUTILS_VERSION.tar.bz2`;
    if ($? != 0) {
  	print "\e[31mError: binutils files failed to download!\e[0m\n";
      return 1;
    }
  }

  if ($already_cached == 0) {
    # Ask the user if he wants to keep the cached files.
    print "\e[32mThe main compiler tarballs can be kept on disk, in case a new compiler build is needed. This will mean that new compiler builds will be faster, but will consume roughly 50MB of disk space.\nCache compiler files? [yes]\e[0m: ";
    $delete_cache = 1 if (<STDIN> =~ m/n/i);
  }
  return 0;
}

# Extract the compiler - assumed download() called and succeeded.
sub extract {
  `mkdir ./compilers/tmp_build` unless (-d "./compilers/tmp_build");

  `cp ./compilers/dl_cache/* ./compilers/tmp_build`;
  return 1 if $? != 0;

  print "\e[32mUn-bzip2-ing downloaded files...\e[0m\n";
  `bunzip2 ./compilers/tmp_build/*`;
  return 1 if $? != 0;

  print "\e[32mExtracting downloaded files [1/3]...\e[0m\n";
  `cd ./compilers/tmp_build/; tar -xf gcc-core-$COMPILER_VERSION.tar`;
  return 1 if $? != 0;

  print "\e[32mExtracting downloaded files [2/3]...\e[0m\n";
  `cd ./compilers/tmp_build/; tar -xf gcc-g++-$COMPILER_VERSION.tar`;
  return 1 if $? != 0;

  print "\e[32mExtracting downloaded files [3/3]...\e[0m\n";
  `cd ./compilers/tmp_build/; tar -xf binutils-$BINUTILS_VERSION.tar`;
  return 1 if $? != 0;

  `rm ./compilers/tmp_build/*.tar*`;
  return 1 if $? != 0;

  `rm -r ./compilers/dl_cache` if $delete_cache == 1;

  return 0;
}

# Configure, make and make install the compiler.
sub install {
  my ($arch) = @_;
  
  `mkdir ./compilers/$arch` unless -d "./compilers/$arch";

  `mkdir -p ./compilers/tmp_build/build_binutils`;
  `mkdir -p ./compilers/tmp_build/build_gcc`;

  print "\e[32mBinutils: \e[0m\e[32;1mConfiguring...\e[0m ";
  my ($pwd) = `pwd` =~ m/^[ \n]*(.*?)[ \n]*$/;
  `export PREFIX=$pwd/compilers; export TARGET=$arch; cd ./compilers/tmp_build/build_binutils/; ../binutils-$BINUTILS_VERSION/configure --target=\$TARGET --prefix=\$PREFIX --disable-nls >/tmp/binutils-configure.out 2>/tmp/binutils-configure.err`;
  if ($? != 0) {print "\e[31mFAIL (Log file at /tmp/binutils-configure.{out|err})\e[0m"; return 1;}
  print "\n\e[32;1mCompiling...\e[0m ";
  `export PREFIX=$pwd/compilers; export TARGET=$arch; cd ./compilers/tmp_build/build_binutils/; make all >/tmp/binutils-make.out 2>/tmp/binutils-make.err`;
  if ($? != 0) {print "\e[31mFAIL (Log file at /tmp/binutils-make.{out|err})\e[0m"; return 1;}
  print "\n\e[32;1mInstalling...\e[0m ";
  `export PREFIX=$pwd/compilers; export TARGET=$arch; cd ./compilers/tmp_build/build_binutils/; make install >/tmp/binutils-make-install.out 2>/tmp/binutils-make-install.err`;
  if ($? != 0) {print "\e[31mFAIL (Log file at /tmp/binutils-make-install.{out|err})\e[0m\n"; return 1;}
  print "\n";

  print "\e[32mGCC: \e[0m\e[32;1mConfiguring...\e[0m ";
  `export PREFIX=$pwd/compilers; export TARGET=$arch; cd ./compilers/tmp_build/build_gcc/; ../gcc-$COMPILER_VERSION/configure --target=\$TARGET --prefix=\$PREFIX --disable-nls --enable-languages=c,c++ --without-headers --without-newlib >/tmp/gcc-configure.out 2>/tmp/gcc-configure.err`;
  if ($? != 0) {print "\e[31mFAIL (Log file at /tmp/gcc-configure.{out|err})\e[0m"; return 1;}
  print "\n\e[32;1mCompiling...\e[0m ";
  `export PREFIX=$pwd/compilers; export TARGET=$arch; cd ./compilers/tmp_build/build_gcc/; make all-gcc >/tmp/gcc-make.out 2>/tmp/gcc-make.err`;
  if ($? != 0) {print "\e[31mFAIL (Log file at /tmp/gcc-make.{out|err})\e[0m"; return 1;}
  print "\n\e[32;1mInstalling...\e[0m ";
  `export PREFIX=$pwd/compilers; export TARGET=$arch; cd ./compilers/tmp_build/build_gcc/; make install-gcc >/tmp/gcc-make-install.out 2>/tmp/gcc-make-install.err`;
  if ($? != 0) {print "\e[31mFAIL (Log file at /tmp/gcc-make-install.{out|err})\e[0m\n"; return 1;}
  print "\n";

  print "\e[32mCleaning up...\e[0m\n";
  `rm -rf ./compilers/tmp_build`;

  return 0;
}

#####################################################################################################
# Script start.

die("No target given!") unless scalar @ARGV > 0;

my $target = $ARGV[0];
if (is_installed($target)) {
  print "\e[32mTarget $target has a suitable compiler already installed.\e[0m\n";
  exit 0;
}

print "\e[32mTarget $target does not have a suitable compiler installed. One must be installed before the make process can continue. Install now? [yes]\e[0m: ";
exit 1 if <STDIN> =~ m/n/i;

if (download() != 0) {
  print "\e[31mFATAL ERROR: Script cannot continue.\e[0m\n";
  exit 1;
}

if (extract() != 0) {
  print "\e[31mFATAL ERROR: Script cannot continue.\e[0m\n";
  exit 1;
}

if (install($target) != 0) {
  print "\e[31mFATAL ERROR: Script cannot continue.\e[0m\n";
  exit 1;
}

exit 0;

