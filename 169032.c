static void k_meta(struct vc_data *vc, unsigned char value, char up_flag)
{
	if (up_flag)
		return;

	if (vc_kbd_mode(kbd, VC_META)) {
		put_queue(vc, '\033');
		put_queue(vc, value);
	} else
		put_queue(vc, value | 0x80);
}