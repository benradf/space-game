#!/bin/bash

# Process args.
while getopts "p:n:i:j:l:" arg; do
    case $arg in
        p) path="$OPTARG" ;;
        n) linkname="$OPTARG" ;;
        i) incpath=`echo "$OPTARG" | sed 's/\([^ \t]\+\)[ \t]*/\\\\\n\t-I\1 /g'` ;;
        j) libpath=`echo "$OPTARG" | sed 's/\([^ \t]\+\)[ \t]*/\\\\\n\t-L\1 /g'` ;;
        l) libs=`echo "$OPTARG" | sed 's/\([^ \t]\+\)[ \t]*/-l\1 /g'` ;;
    esac
done

phony=".PHONY: all clean dist"
islibs=`echo $linkname | sed '/^lib.*\.a$/!d'`
islibd=`echo $linkname | sed '/^lib.*\.so$/!d'`
if [ $islibd ]; then
    picflag="-fPIC "
fi
if [ $path ]; then
    cd $path
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
else
    echo "BIN=$linkname"
fi
echo "NAME=`echo $linkname | sed 's/^lib\(.*\).\(a\|so\)$/\1/'`"
echo 'ROOT?=../..'
echo 'BUILDROOT?=$(ROOT)/build'
echo 'PLATFORM?=linux64'
echo 'BUILDPATH=$(BUILDROOT)/$(PLATFORM)/$(NAME)'
echo "OBJS=$sources" | sed 's/ \(\w*\)\.cpp/ \\\n\t\$(BUILDPATH)\/\1\.o/g'
echo 'CFLAGS:=$(CFLAGS) '"$incpath"
echo 'CXXFLAGS:=$(CXXFLAGS) '"$incpath"
echo 'LDFLAGS:=$(LDFLAGS) '"$libpath"
echo 'RANLIB?=ranlib'
echo

# Generate all target.
if [ $islibs ] || [ $islibd ]; then
    echo 'all: $(BUILDPATH)/$(LIB)'
else
    echo 'all: $(BUILDPATH)/$(BIN)'
fi
echo

# Generate clean target.
echo 'clean:'
echo '	rm $(BUILDPATH)/*'
echo

# Generate linking target.
echo "# Link object files."
if [ $islibs ]; then
    echo '$(BUILDPATH)/$(LIB): $(OBJS)'
    echo '	$(AR) cru $(BUILDPATH)/$(LIB) $(OBJS)'
    echo '	$(RANLIB) $(BUILDPATH)/$(LIB)'
elif [ $islibd ]; then
    echo '$(BUILDPATH)/$(LIB): $(OBJS)'
    echo '	$(CXX) -shared -Wl,-soname,$(LIB) $(LDFLAGS) -o $(BUILDPATH)/$(LIB) $(OBJS)'" $libs"
else
    echo '$(BUILDPATH)/$(BIN): $(OBJS)'
    echo '	$(CXX) $(LDFLAGS) -o $(BUILDPATH)/$(BIN) $(OBJS)'" $libs"
fi
echo

# Generate compilation targets.
echo "# Compile source files."
for file in $sources; do
	obj=`echo $file | sed 's/^\(.*\)\.cpp$/\$(BUILDPATH)\/\1\.o/'`
    echo "$obj: $file"
	echo '	$(CXX) '"$picflag"'-c $(CXXFLAGS)'" -o $obj $file"
done
echo

# Generate header dependencies.
echo "# Header dependencies."
for file in `ls *.?pp`; do
	echo -n "$file:"
    sed -n 's/^[ \t]*#include.*\"\(.*\)\"[ \t]*$/\1/p;' <$file |
    while read inc; do
        echo -n " $inc"
    done 
	echo -e "\n	@touch $file"
done
echo

echo "$phony"
