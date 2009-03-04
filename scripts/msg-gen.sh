#!/bin/bash
# Auto-generate message definitions.

# Variables.
SPEC="msg.spec"
AUTHOR="Ben Radford"
MSGHDR="messages.hpp"
MSGSRC="messages.cpp"
HANDLERHDR="msghandler.hpp"
HANDLERSRC="msghandler.cpp"
MSGDESC="Auto-generated message definitions."
HANDLERDESC="Auto-generated message handler."

# Output file comments.
function file-comments() {
    echo "/// \\file $1"
    echo "/// \\brief $2"
    echo "/// \\author $AUTHOR"
    DAY=`date +%-d`
    DAYMOD=$(($DAY % 10))
    if [ DAYMOD == 1 ]; then
        DAYORD="${DAY}st"
    elif [ DAYMOD == 2 ]; then
        DAYORD="${DAY}nd"
    elif [ DAYMOD == 3 ]; then
        DAYORD="${DAY}rd"
    else
        DAYORD="${DAY}th"
    fi
    echo "/// \\date $DAYORD `date +"%B %Y"`"
    echo "///"
    echo "/// Copyright (c) `date +%Y` $AUTHOR. All rights reserved."
    echo "///"
    echo
    echo
}

# Add opening header guard.
function open-header-guard() {
    GUARDNAME=`echo $1 | tr [a-z] [A-Z] | sed 's/\./_/'`
    echo "#ifndef $GUARDNAME"
    echo "#define $GUARDNAME"
    echo
    echo
}

# Add closing header guard.
function close-header-guard() {
    GUARDNAME=`echo $1 | tr [a-z] [A-Z] | sed 's/\./_/'`
    echo "#endif  // $GUARDNAME"
    echo
}

# Change directory.
cd common/src/server

# Remove previously generated files.
rm $MSGHDR $MSGSRC $HANDLERHDR $HANDLERSRC

# Open specification.
exec 3<>$SPEC

# Open message header.
exec 4<>$MSGHDR 1>&4
file-comments "$MSGHDR" "$MSGDESC"
open-header-guard "$MSGHDR"
echo "#include \"typedefs.hpp\""
echo
echo
echo "namespace msg {"
echo
echo
echo "class MessageHandler;"
echo
echo
echo "class Message {"
echo "    public:"
echo "        virtual ~Message();"
echo "        virtual void dispatch(MessageHandler& handler) = 0;"
echo
echo "    private:"
echo
echo "};"
echo
echo

# Open message source.
exec 5<>$MSGSRC 1>&5
file-comments "$MSGSRC" "$MSGDESC"
echo "#include \"$MSGHDR\""
echo "#include \"$HANDLERHDR\""
echo
echo
echo "msg::Message::~Message()"
echo "{"
echo
echo "}"
echo
echo

# Open handler header.
exec 6<>$HANDLERHDR 1>&6
file-comments "$HANDLERHDR" "$HANDLERDESC"
open-header-guard "$HANDLERHDR"
echo "#include \"typedefs.hpp\""
echo
echo
echo "namespace msg {"
echo
echo
echo "class MessageHandler {"
echo "    public:"
echo "        virtual ~MessageHandler();"

# Open handler source.
exec 7<>$HANDLERSRC 1>&7
file-comments "$HANDLERSRC" "$HANDLERDESC"
echo "#include \"$HANDLERHDR\""
echo
echo
echo "////////// msg::MessageHandler //////////"
echo

# Write message defs.
while read MSG <&3; do
    MSGNAME=`echo $MSG | sed 's/^\(.*\)(.*)$/\1/'`
    ARGLIST=`echo $MSG | sed 's/^.*(\(.*\))$/\1/'`
    ARGS=`echo $ARGLIST | sed 's/, /\n/g;s/&//g'`

    # Message header.
    exec 1>&4
    echo "class $MSGNAME : public Message {"
    echo "    public:"
    echo "        $MSG;"
    echo "        virtual ~$MSGNAME();"
    echo "        virtual void dispatch(MessageHandler& handler);"
    echo
    echo "    private:"
    echo -e "$ARGS" |
    while read ARG; do
        echo "        $ARG;" |
        sed 's/^\(.*\) \(.*\)$/\1 _\2/'
    done
    echo "};"
    echo
    echo

    # Message source.
    exec 1>&5
    echo "////////// msg::$MSGNAME //////////"
    echo
    echo "msg::$MSGNAME::$MSG :"
    echo -n "    "
    echo -e "$ARGS" | while read ARG; do
        echo -n $ARG | sed 's/^\(.*\) \(.*\)$/_\2(\2), /'
    done | sed 's/\(.*\), $/\1/'
    echo
    echo "{"
    echo
    echo "}"
    echo "msg::$MSGNAME::~$MSGNAME()"
    echo "{"
    echo
    echo "}"
    echo
    echo "void msg::$MSGNAME::dispatch(MessageHandler& handler)"
    echo "{"
    echo -n "    handler.handle$MSGNAME("
    echo -e "$ARGS" | while read ARG; do
        echo -n $ARG | sed 's/^\(.*\) \(.*\)$/_\2, /'
    done | sed 's/\(.*\), $/\1/'
    echo ");"
    echo "}"
    echo
    echo

    # Handler header.
    exec 1>&6
    echo "        virtual void handle$MSG;"

    # Handler source.
    exec 1>&7
    echo "void msg::MessageHandler::handle$MSG"
    echo "{"
    echo
    echo "}"
    echo
done

# Close message header.
exec 1>&4 4>&-
echo "}  // namespace msg"
echo
echo
close-header-guard "$MSGHDR"

# Close message source.
exec 1>&5 5>&-

# Close handler header.
exec 1>&6 6>&-
echo "};"
echo
echo
echo "}  // namespace msg"
echo
echo
close-header-guard "$HANDLERHDR"

# Close handler source.
exec 1>&7 5>&-


