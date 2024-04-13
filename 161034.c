printf_channel_usage()
{
	lprintf(LOG_NOTICE,
"Channel Commands: authcap   <channel number> <max privilege>");
	lprintf(LOG_NOTICE,
"                  getaccess <channel number> [user id]");
	lprintf(LOG_NOTICE,
"                  setaccess <channel number> "
"<user id> [callin=on|off] [ipmi=on|off] [link=on|off] [privilege=level]");
	lprintf(LOG_NOTICE,
"                  info      [channel number]");
	lprintf(LOG_NOTICE,
"                  getciphers <ipmi | sol> [channel]");
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"Possible privilege levels are:");
	lprintf(LOG_NOTICE,
"   1   Callback level");
	lprintf(LOG_NOTICE,
"   2   User level");
	lprintf(LOG_NOTICE,
"   3   Operator level");
	lprintf(LOG_NOTICE,
"   4   Administrator level");
	lprintf(LOG_NOTICE,
"   5   OEM Proprietary level");
	lprintf(LOG_NOTICE,
"  15   No access");
}