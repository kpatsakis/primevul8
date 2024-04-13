char *hid_resolv_usage(unsigned usage, struct seq_file *f) {
	const struct hid_usage_entry *p;
	char *buf = NULL;
	int len = 0;

	buf = resolv_usage_page(usage >> 16, f);
	if (IS_ERR(buf)) {
		pr_err("error allocating HID debug buffer\n");
		return NULL;
	}


	if (!f) {
		len = strlen(buf);
		snprintf(buf+len, max(0, HID_DEBUG_BUFSIZE - len), ".");
		len++;
	}
	else {
		seq_printf(f, ".");
	}
	for (p = hid_usage_table; p->description; p++)
		if (p->page == (usage >> 16)) {
			for(++p; p->description && p->usage != 0; p++)
				if (p->usage == (usage & 0xffff)) {
					if (!f)
						snprintf(buf + len,
							max(0,HID_DEBUG_BUFSIZE - len - 1),
							"%s", p->description);
					else
						seq_printf(f,
							"%s",
							p->description);
					return buf;
				}
			break;
		}
	if (!f)
		snprintf(buf + len, max(0, HID_DEBUG_BUFSIZE - len - 1),
				"%04x", usage & 0xffff);
	else
		seq_printf(f, "%04x", usage & 0xffff);
	return buf;
}