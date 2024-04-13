static void fn_lastcons(struct vc_data *vc)
{
	/* switch to the last used console, ChN */
	set_console(last_console);
}