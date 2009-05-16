KeyExchange(uint64 key)
Login(string username, uint8 password[16])
Disconnect()
WhoIsPlayer(uint32 playerid)
GetObjectName(uint16 objectid)
PlayerInfo(uint32 playerid, string username)
PlayerInput(uint32 flags)
PrivateMsg(uint32 playerid, string text)
BroadcastMsg(string text)
ObjectEnter(uint16 objectid)
ObjectLeave(uint16 objectid)
ObjectAttach(uint16 objectid)
ObjectName(uint16 objectid, string name)
ObjectUpdatePartial(uint16 objectid, int16 s_x, int16 s_y)
ObjectUpdateFull(uint16 objectid, int16 s_x, int16 s_y, int16 v_x, int16 v_y, uint8 rot, uint8 ctrl)
MsgPubChat(string text)
MsgPrivChat(string text)
MsgSystem(string text)
MsgInfo(string text)
