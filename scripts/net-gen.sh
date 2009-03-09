#!/bin/bash 

SPEC="net.spec"
PROTOCOLHDR="protocol.hpp"
PROTOCOLSRC="protocol.cpp"
PROTOCOLDESC="Network protocol."
TMPFILE="/tmp/netgen_temp"
ARGFILE="/tmp/netgen_argfile"
SERIALISEFILE="/tmp/netgen_serialise"
SEDFMT='^\([[:alnum:]]*\) \([[:alnum:]]*\)\(\[.\+\]\)\?$'

. scripts/code-gen.inc

cd common/src/net
rm $PROTOCOLHDR $PROTOCOLSRC

function net2cpp() {
    case "$1" in
        uint8) CPPTYPE="uint8_t";;
        uint16) CPPTYPE="uint16_t";;
        uint32) CPPTYPE="uint32_t";;
        uint64) CPPTYPE="uint64_t";;
        sint8) CPPTYPE="int8_t";;
        sint16) CPPTYPE="int16_t";;
        sint32) CPPTYPE="int32_t";;
        sint64) CPPTYPE="int64_t";;
        string) CPPTYPE="const char*";;
        real32) CPPTYPE="float";;
        real64) CPPTYPE="double";;
        *) CPPTYPE="unknown_t";;
    esac

    echo "$CPPTYPE"
}

function type-size() {
    case "$1" in
        uint8) SIZE="1";;
        uint16) SIZE="2";;
        uint32) SIZE="4";;
        uint64) SIZE="8";;
        sint8) SIZE="1";;
        sint16) SIZE="2";;
        sint32) SIZE="4";;
        sint64) SIZE="8";;
        real32) SIZE="4";;
        real64) SIZE="8";;
        *) SIZE="0";;
    esac

    echo "$SIZE"
}

function int-bits() {
    echo $1 | sed 's/^u\?int\(..\?\)_t$/\1/'
}

function arg-type() {
    echo $1 | sed "s/$SEDFMT/\1/"
}

function arg-name() {
    echo $1 | sed "s/$SEDFMT/\2/"
}

function arg-array() {
    echo $1 | sed "s/$SEDFMT/\3/"
}

function fun-args() {
    echo -e "$1" | while read ARG; do
        if [ "$ARG" ]; then
            NAME=`arg-name "$ARG"`
            ARRAY=`arg-array "$ARG"`
            NETTYPE=`arg-type "$ARG"`
            CPPTYPE=`net2cpp "$NETTYPE"`

            if [ "$ARRAY" ]; then
                NAME="(&$NAME)$ARRAY"
            fi

            echo -n "$CPPTYPE $NAME, "
        fi
    done | sed 's/\(.*\), /\1/'
}

# $1 - ident, $2 - typecode, $3 - argspec, $4 - header, 
# $5 - arrayfunc, $6 - stringfunc, $7 - otherfunc, $8 - footer, $9 - endianconv
function process-args() {
    PLACEHOLDERS="0"
    TOTALBYTES="0"
    INDEX="0"

    exec 7<>$ARGFILE 8>&1 1<>$SERIALISEFILE
    echo "$3" >&7
    exec 7>&- 7<>$ARGFILE

    $4 "$1" "$2" "<~0~>"

    while read ARG <&7; do
        if [ "$ARG" ]; then
            NAME=`arg-name "$ARG"`
            ARRAY=`arg-array "$ARG"`
            NETTYPE=`arg-type "$ARG"`
            CPPTYPE=`net2cpp "$NETTYPE"`
            INTBITS=`int-bits "$CPPTYPE"`
            TYPESIZE=`type-size "$NETTYPE"`
            ORIGNAME="$NAME"

            if [ "$ARRAY" ]; then
                NAME="$NAME[i]"
            fi

            NAME="`$9 "$INTBITS"`($NAME)"

            if [ "$ARRAY" ]; then
                SIZE=`echo $ARRAY | sed 's/\[\(.*\)\]/\1/'`
                TOTALBYTES=$(($SIZE * $TYPESIZE + $TOTALBYTES))
                $5 "$1" "$NAME" "$CPPTYPE" "$SIZE" "$ORIGNAME"
            elif [ "$NETTYPE" = "string" ]; then
                TOTALBYTES=$(($TOTALBYTES + 2))
                PLACEHOLDERS="$PLACEHOLDERS $TOTALBYTES"
                $6 "$1" "$NAME" "<~$TOTALBYTES~>"
            else
                TOTALBYTES=$(($TOTALBYTES + $TYPESIZE))
                $7 "$1" "$NAME" "$CPPTYPE" "$ORIGNAME"
            fi

            INDEX=$(($INDEX + 1))
        fi
    done

    $8 "$1" "$2" "$TOTALBYTES"

    exec 7<&- 1>&8 8>&-

    SEDREPLACE=""

    for P in $PLACEHOLDERS; do
        Q=$(($TOTALBYTES - $P))
        Q_HEX=`printf "0x%02x" "$Q"`
        SEDREPLACE="${SEDREPLACE}s/<~$P~>/$Q_HEX/;"
    done

    sed "$SEDREPLACE" <$SERIALISEFILE

    rm $ARGFILE $SERIALISEFILE

}

# $1 - indent, $2 - typecode, $3 - remaining
function serialise-header() {
    printf "%$1sENetPacket* packet = enet_packet_create(" ""
    printf     "0, 1024, ENET_PACKET_FLAG_RELIABLE);\n" ""
    printf "%$1senet_uint8* offset = packet->data;\n" ""
    printf "%$1s*reinterpret_cast<uint8_t*>(offset) = 0x%02x;\n" "" "$2"
    printf "%$1suint16_t len = 0;\n" ""
    printf "%$1soffset += 0x01;\n" ""
}

# $1 - indent, $2 - name, $3 - cpptype, $4 - size, $5 - origname
function serialise-array() {
    printf "%$1sfor (int i = 0; i < $4; i++)\n" ""
    printf "%$1s    reinterpret_cast<%s*>(offset)[i] = %s;\n" "" "$3" "$2"
    printf "%$1soffset += sizeof(%s);\n" "" "$5"
}

# $1 - indent, $2 - name, $3 - remaining
function serialise-string() {
    printf "%$1slen = std::min(strlen(%s), MAXSTRLEN);\n" "" "$2"
    printf "%$1sif ((offset + len + %s > packet->data + packet->dataLength) && \n" "" "$3"
    printf "%$1s        (enet_packet_resize(packet, (packet->dataLength + len + %s) * 2) != 0)) {\n" "" "$3"
    printf "%$1s    Log::log->warn(\"Network: serialise %s: enet_packet_resize failed\");\n" "" "$LOGNAME"
    printf "%$1s    return;\n" ""
    printf "%$1s}\n" ""
    printf "%$1s*reinterpret_cast<uint16_t*>(offset) = htons(len);\n" ""
    printf "%$1smemcpy(offset += 0x02, %s, len);\n" "" "$2"
    printf "%$1soffset += len;\n" ""
}

# $1 - indent, $2 - name, $3 - cpptype, $4 - origname
function serialise-other() {
    printf "%$1s*reinterpret_cast<%s*>(offset) = %s;\n" "" "$3" "$2"
    printf "%$1soffset += sizeof(%s);\n" "" "$3"
}

# $1 - indent, $2 - typecode, $3 - totalbytes
function serialise-footer() {
    printf "%$1senet_packet_resize(packet, offset - packet->data);\n" ""
    printf "%$1ssendPacket(packet);\n" ""
}

# $1 - bits
function serialise-endian() {
    case "$1" in
        "16") echo "htons";;
        "32") echo "htonl";;
        "64") echo "htonq";;
    esac
}

# $1 - indent, $2 - typecode, $3 - argspec
function serialise() {
    process-args "$1" "$2" "$3" \
        "serialise-header" \
        "serialise-array" \
        "serialise-string" \
        "serialise-other" \
        "serialise-footer" \
        "serialise-endian"
}

# $1 - indent, $2 - typecode, $3 - remaining
function deserialise-header() {
    CALLHANDLE="handle$NAME("
    printf "%$1scase 0x%02x: {\n" "" "$2"
    printf "%$1s    if (offset + %s > packet->data + packet->dataLength) {\n" "" "$3"
    printf "%$1s        Log::log->warn(\"Network: deserialise %s: malformed packet\");\n" "" "$LOGNAME"
    printf "%$1s        return;\n" ""
    printf "%$1s    }\n" ""
}

# $1 - indent, $2 - name, $3 - cpptype, $4 - size, $5 - origname
function deserialise-array() {
    printf "%$1s    %s* %s = reinterpret_cast<%s*>(offset);\n" "" "$3" "$5" "$3"
    printf "%$1s    offset += sizeof(%s);\n" "" "$5"
    CALLHANDLE="${CALLHANDLE}reinterpret_cast<$3(&)[$4]>(*$5), "
}

# $1 - indent, $2 - name, $3 - remaining
function deserialise-string() {
    printf "%$1s    len = ntohs(*reinterpret_cast<uint16_t*>(offset));\n" ""
    printf "%$1s    offset += 0x02;\n" ""
    printf "%$1s    if (offset + len + %s > packet->data + packet->dataLength) {\n" "" "$3"
    printf "%$1s        Log::log->warn(\"Network: deserialise %s: malformed packet\");\n" "" "$LOGNAME"
    printf "%$1s        return;\n" ""
    printf "%$1s    }\n" ""
    printf "%$1s    char* %s = reinterpret_cast<char*>(offset - 1);\n" "" "$2"
    printf "%$1s    memmove(%s, offset, len);\n" "" "$2"
    printf "%$1s    %s[len] = '\\\0';\n" "" "$2"
    printf "%$1s    offset += len;\n" ""
    CALLHANDLE="$CALLHANDLE$2, "
}

# $1 - indent, $2 - name, $3 - cpptype, $4 - origname
function deserialise-other() {
    printf "%$1s    %s %s = *reinterpret_cast<%s*>(offset);\n" "" "$3" "$4" "$3"
    printf "%$1s    offset += sizeof(%s);\n" "" "$3"
    CALLHANDLE="$CALLHANDLE$2, "
}

# $1 - indent, $2 - typecode, $3 - remaining
function deserialise-footer() {
    CALLHANDLE=`echo "$CALLHANDLE" | sed 's/^\(.*\), $/\1/'`
    printf "%$1s    %s);\n" "" "$CALLHANDLE"
    printf "%$1s    } break;\n" ""
}

# $1 - bits
function deserialise-endian() {
    case "$1" in
        "16") echo "ntohs";;
        "32") echo "ntohl";;
        "64") echo "ntohq";;
    esac
}

# $1 - indent, $2 - typecode, $3 - argspec
function deserialise() {
    process-args "$1" "$2" "$3" \
        "deserialise-header" \
        "deserialise-array" \
        "deserialise-string" \
        "deserialise-other" \
        "deserialise-footer" \
        "deserialise-endian"
}

# Open protocol header.
exec 4<>$PROTOCOLHDR 1>&4
file-comments "$PROTOCOLHDR" "$PROTOCOLDESC"
open-header-guard "$PROTOCOLHDR"
echo
echo
echo "#include <enet/enet.h>"
echo "#include <stdint.h>"
echo
echo
echo "namespace net {"
echo
echo
echo "static const size_t MAXSTRLEN = 1024;"
echo
echo
echo "class ProtocolUser {"
echo "    public:"
echo "        virtual ~ProtocolUser();"
echo
echo "        virtual void sendPacket(ENetPacket* packet) = 0;"
echo "        void handlePacket(ENetPacket* packet);"
echo

# Open protocol source.
exec 5<>$PROTOCOLSRC 1>&5
file-comments "$PROTOCOLSRC" "$PROTOCOLDESC"
echo "#include \"$PROTOCOLHDR\""
echo "#include <core.hpp>"
echo "#include <memory.h>"
echo
echo
echo "#define htonq(x) x  // needs implementing"
echo "#define ntohq(x) x  // needs implementing"
echo
echo "#define CHECK_WRITE_OFFSET(pkt, off, type) { \\"
echo "    if ((off + sizeof(type) > pkt->data + pkt->dataLength) && \\"
echo "            (enet_packet_resize(pkt, pkt->dataLength * 2) != 0)) \\"
echo "        return; }"
echo
echo
echo "#define CHECK_READ_OFFSET(pkt, off, type) { \\"
echo "    if (off + sizeof(type) > pkt->data + pkt->dataLength) \\"
echo "        return; }"
echo
echo
echo "////////// net::ProtocolUser //////////"
echo
echo "net::ProtocolUser::~ProtocolUser()"
echo "{"
echo
echo "}"
echo
echo "void net::ProtocolUser::handlePacket(ENetPacket* packet)"
echo "{"
echo "    enet_uint8* offset = packet->data;"
echo "    uint8_t typecode = *offset++;"
echo "    uint16_t len = 0;"
echo
echo "    switch (typecode) {"

# Tmp file to help with generation.
exec 6<>$TMPFILE

TYPECODE="1"
exec 3>&- 3<>$SPEC
while read LINE <&3; do
    NAME=`echo $LINE | sed 's/^\(.*\)(.*)$/\1/'`
    ARGS=`echo $LINE | sed 's/^.*(\(.*\))$/\1/;s/, /\n/g'`
    FUNCTION="$NAME(`fun-args \"$ARGS\"`)"
    LOGNAME="$NAME"
    
    exec 1>&4
    echo "        void send$FUNCTION;" 
    echo "        virtual void handle$FUNCTION = 0;"
    echo

    exec 1>&5
    deserialise "4" "$TYPECODE" "$ARGS"

    exec 1>&6
    echo "void net::ProtocolUser::send$FUNCTION"
    echo "{"
    serialise "4" "$TYPECODE" "$ARGS"
    TYPECODE=$(($TYPECODE + 1))
    echo "}"
    echo
done

# Close protocol header.
exec 1>&4 4>&-
echo "};"
echo
echo
echo "}  // namespace net"
echo
echo
close-header-guard "$PROTOCOLHDR"

# Close tmp file.
exec 6>&-

# Close protocol source.
exec 1>&5 5>&-
echo "    }"
echo "}"
echo
cat $TMPFILE
rm $TMPFILE











