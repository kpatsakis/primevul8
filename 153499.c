static void hid_resolv_event(__u8 type, __u16 code, struct seq_file *f)
{
	seq_printf(f, "%s.%s", events[type] ? events[type] : "?",
		names[type] ? (names[type][code] ? names[type][code] : "?") : "?");
}