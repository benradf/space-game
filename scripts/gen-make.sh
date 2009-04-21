#!/bin/bash

# Process args.
while getopts "p:n:i:j:l:h:r:f:" arg; do
    case $arg in
        p) path="$OPTARG" ;;
        n) linkname="$OPTARG" ;;
        i) incpath=`echo "$OPTARG" | sed 's/\([^ \t]\+\)[ \t]*/\\\\\n\t-I\1 /g'` ;;
        j) libpath=`echo "$OPTARG" | sed 's/\([^ \t]\+\)[ \t]*/\\\\\n\t-L\1 /g'` ;;
        l) libs=`echo "$OPTARG" | sed 's/\([^ \t]\+\)[ \t]*/-l\1 /g'` ;;
        h) headers="$OPTARG" ;;
        r) root="$OPTARG" ;;
        f) flags="$OPTARG" ;;
    esac
done

if [ -z $linkname ]; then
    echo "gen-make.sh: must use -n to specify a linkname." 1>&2
    exit 1
fi

linkname=`echo $linkname`
phony=".PHONY: all clean dist"
islibs=`echo $linkname | sed '/^lib.*\.a$/!d'`
islibd=`echo $linkname | sed '/^lib.*\.so$/!d'`
if [ $islibd ]; then
    picflag="-fPIC "
fi
if [ $path ]; then
    cd $path
fi
if [ -z $root ]; then
    root=$PWD
fi

# Get list of source files.
for file in `ls *.cpp`; do
    sources="$sources $file"
done

# Generate header.
echo "# --- Makefile for $linkname ---"
echo "#   `date`"
echo

# Generate make variables.
echo "# Set variables."
if [ $islibs ] || [ $islibd ]; then
    echo "LIB=$linkname"
    installdir=lib
else
    echo "BIN=$linkname"
    installdir=bin
fi
SEDNAME='s/^lib\(.*\)\.\(a\|so\)$/\1/;s/^\(.*\)\.\(exe\|dll\)/\1/'
echo "NAME=`echo $linkname | sed $SEDNAME`"
echo 'ROOT?='"$root"
echo 'BUILDROOT?=$(ROOT)'
echo 'PLATFORM?=linux'
echo 'PREFIX=$(ROOT)/$(PLATFORM)'
echo 'BUILDPATH=$(BUILDROOT)/$(PLATFORM)/tmp/$(NAME)'
echo "OBJS=$sources" | sed 's/ \(\w*\)\.cpp/ \\\n\t\$(BUILDPATH)\/\1\.o/g'
echo 'CFLAGS:=$(CFLAGS) '"$flags $incpath"
echo 'CXXFLAGS:=$(CXXFLAGS) '"$flags $incpath"
echo 'LDFLAGS:=$(LDFLAGS) '"$libpath"
echo 'RANLIB?=ranlib'

# Generate library dependencies.
echo -n 'LIBDEPS='
searchpaths=`echo $libpath | sed 's/\\\//g' | sed 's/-L//g' | tr -d '\n'`
for lib in $libs; do
    libname=`echo $lib | sed 's/-l\(.*\)/lib\1.a/'`
    for path in $searchpaths; do
        if [ -f $path/$libname ]; then
            echo -e -n ' \\\n\t'"$path/$libname"
        fi
    done
done
echo -e '\n'

# Generate all target.
if [ $islibs ] || [ $islibd ]; then
    echo 'all: $(BUILDPATH)/$(LIB)'
else
    echo 'all: $(BUILDPATH)/$(BIN)'
fi
echo

# Generate clean target.
echo 'clean:'
echo '	rm -f $(BUILDPATH)/*'
echo

# Generate install target.
echo '# Install link target and headers.'
if [ $islibs ] || [ $islibd ]; then
    echo 'install: $(BUILDPATH)/$(LIB)'" $headers"
    echo '	cp -uv $(BUILDPATH)/$(LIB) $(PREFIX)/'"$installdir"
else
    echo 'install: $(BUILDPATH)/$(BIN)'" $headers"
    echo '	cp -uv $(BUILDPATH)/$(BIN) $(PREFIX)/'"$installdir"
fi
echo

# Generate Link target.
echo '# Link object files.'
if [ $islibs ]; then
    echo '$(BUILDPATH)/$(LIB): $(OBJS) $(LIBDEPS)'
    echo '	$(AR) cru $(BUILDPATH)/$(LIB) $(OBJS)'
    echo '	$(RANLIB) $(BUILDPATH)/$(LIB)'
elif [ $islibd ]; then
    echo '$(BUILDPATH)/$(LIB): $(OBJS) $(LIBDEPS)'
    echo '	$(CXX) -shared -Wl,-soname,$(LIB) $(LDFLAGS) -o $(BUILDPATH)/$(LIB) $(OBJS)'" $libs"
else
    echo '$(BUILDPATH)/$(BIN): $(OBJS) $(LIBDEPS)'
    echo '	$(CXX) $(LDFLAGS) -o $(BUILDPATH)/$(BIN) $(OBJS)'" $libs"
fi
echo

function obj-name() {
    echo $1 | sed 's/^\(.*\)\.cpp$/\$(BUILDPATH)\/\1\.o/'
}

# Generate compilation targets.
echo "# Compile source files."
for file in $sources; do
    obj=`obj-name $file`
    echo "$obj: $file"
	echo '	$(CXX) '"$picflag"'-c $(CXXFLAGS)'" -o $obj $file"
done
echo

# Generate header dependencies.
echo "# Header dependencies."
cflags=`echo $incpath | sed 's/\\\//g' | tr -d '\n'`
for file in $sources; do
    echo -n '$(BUILDPATH)/'
    gcc -MM $cflags $file
    if [ $? = 1 ]; then
        echo "$file:" | sed s/\.cpp/\.o/
    fi
    echo
done

echo "$phony"

exit 0

