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
SEDMSGTYPE="s/^\(.*\)(.*)$/\1/;s/^\([[:alnum:]]*\)_.*$/\1/"
SEDMSGNAME="s/^\(.*\)(.*)$/\1/;s/_//g"
SEDARGLIST="s/^.*(\(.*\))$/\1/"
. scripts/code-gen.inc

# Write message types.
function write-message-types() {
    exec 1>&4 3<&- 3<>$SPEC
    echo "enum MsgType {"
    MAXWIDTH="1"
    while read MSG <&3; do
        MSGNAME=`echo $MSG | sed "$SEDMSGNAME"`
        MSGTYPE=`echo $MSG | sed "$SEDMSGTYPE" | tr [a-z] [A-Z]`
        if [ ${#MSGTYPE} -gt $MAXWIDTH ]; then
            MAXWIDTH=${#MSGTYPE}
        fi
    done
    FLAGVALUE="1"
    exec 3<&- 3<>$SPEC
    while read MSG <&3; do
        MSGNAME=`echo $MSG | sed "$SEDMSGNAME"`
        echo $MSG | sed "$SEDMSGTYPE" | tr [a-z] [A-Z]
    done | sort -u |
    while read MSGTYPE; do
        printf "    MSG_%-${MAXWIDTH}s = 0x%04x,\n" $MSGTYPE $FLAGVALUE
        FLAGVALUE=$(($FLAGVALUE * 2))
    done
    echo "};"
}

# Change directory.
cd common/src/server

# Remove previously generated files.
rm $MSGHDR $MSGSRC $HANDLERHDR $HANDLERSRC

# Open message header.
exec 4<>$MSGHDR 1>&4
file-comments "$MSGHDR" "$MSGDESC"
open-header-guard "$MSGHDR"
echo "#include <memory>"
echo "#include \"typedefs.hpp\""
echo
echo
echo "namespace msg {"
echo
echo
echo "class MessageHandler;"
echo
echo
write-message-types
echo
echo
echo "class Message {"
echo "    public:"
echo "        virtual ~Message();"
echo "        virtual std::auto_ptr<Message> clone() const = 0;"
echo "        virtual void dispatch(MessageHandler& handler) = 0;"
echo "        virtual bool matches(int subscription) = 0;"
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
echo "////////// msg::Message //////////"
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
echo "msg::MessageHandler::~MessageHandler()"
echo "{"
echo
echo "}"
echo

# Write message defs.
exec 3<&- 3<>$SPEC
while read MSG <&3; do
    MSGNAME=`echo $MSG | sed "$SEDMSGNAME"`
    ARGLIST=`echo $MSG | sed "$SEDARGLIST"`
    ARGS=`echo $ARGLIST | sed 's/, /\n/g;s/&//g'`
    MSGTYPE=`echo $MSG | sed "$SEDMSGTYPE" | tr [a-z] [A-Z]`
    MSG="$MSGNAME($ARGLIST)"

    # Message header.
    exec 1>&4
    echo "class $MSGNAME : public Message {"
    echo "    public:"
    echo "        $MSG;"
    echo "        virtual ~$MSGNAME();"
    echo "        virtual std::auto_ptr<Message> clone() const;"
    echo "        virtual void dispatch(MessageHandler& handler);"
    echo "        virtual bool matches(int subscription);"
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
    echo
    echo "msg::$MSGNAME::~$MSGNAME()"
    echo "{"
    echo
    echo "}"
    echo
    echo "std::auto_ptr<msg::Message> msg::$MSGNAME::clone() const"
    echo "{"
    echo "    return std::auto_ptr<Message>(new $MSGNAME(*this));"
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
    echo "bool msg::$MSGNAME::matches(int subscription)"
    echo "{"
    echo "    return ((subscription & MSG_$MSGTYPE) != 0);"
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


