debug_logging_stop(void)
{
if (!debug_file || !debuglog_name[0]) return;

debug_selector = 0;
fclose(debug_file);
debug_file = NULL;
unlink_log(lt_debug);
}