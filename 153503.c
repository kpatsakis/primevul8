static void hid_dump_input_mapping(struct hid_device *hid, struct seq_file *f)
{
	int i, j, k;
	struct hid_report *report;
	struct hid_usage *usage;

	for (k = HID_INPUT_REPORT; k <= HID_OUTPUT_REPORT; k++) {
		list_for_each_entry(report, &hid->report_enum[k].report_list, list) {
			for (i = 0; i < report->maxfield; i++) {
				for ( j = 0; j < report->field[i]->maxusage; j++) {
					usage = report->field[i]->usage + j;
					hid_resolv_usage(usage->hid, f);
					seq_printf(f, " ---> ");
					hid_resolv_event(usage->type, usage->code, f);
					seq_printf(f, "\n");
				}
			}
		}
	}

}