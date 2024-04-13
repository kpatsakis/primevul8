unlink_log(int type)
{
if (type == lt_debug) unlink(CS debuglog_name);
}