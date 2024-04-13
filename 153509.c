void hid_dump_device(struct hid_device *device, struct seq_file *f)
{
	struct hid_report_enum *report_enum;
	struct hid_report *report;
	struct list_head *list;
	unsigned i,k;
	static const char *table[] = {"INPUT", "OUTPUT", "FEATURE"};

	for (i = 0; i < HID_REPORT_TYPES; i++) {
		report_enum = device->report_enum + i;
		list = report_enum->report_list.next;
		while (list != &report_enum->report_list) {
			report = (struct hid_report *) list;
			tab(2, f);
			seq_printf(f, "%s", table[i]);
			if (report->id)
				seq_printf(f, "(%d)", report->id);
			seq_printf(f, "[%s]", table[report->type]);
			seq_printf(f, "\n");
			for (k = 0; k < report->maxfield; k++) {
				tab(4, f);
				seq_printf(f, "Field(%d)\n", k);
				hid_dump_field(report->field[k], 6, f);
			}
			list = list->next;
		}
	}
}