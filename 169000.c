static void fn_bare_num(struct vc_data *vc)
{
	if (!rep)
		chg_vc_kbd_led(kbd, VC_NUMLOCK);
}