void hid_dump_field(struct hid_field *field, int n, struct seq_file *f) {
	int j;

	if (field->physical) {
		tab(n, f);
		seq_printf(f, "Physical(");
		hid_resolv_usage(field->physical, f); seq_printf(f, ")\n");
	}
	if (field->logical) {
		tab(n, f);
		seq_printf(f, "Logical(");
		hid_resolv_usage(field->logical, f); seq_printf(f, ")\n");
	}
	if (field->application) {
		tab(n, f);
		seq_printf(f, "Application(");
		hid_resolv_usage(field->application, f); seq_printf(f, ")\n");
	}
	tab(n, f); seq_printf(f, "Usage(%d)\n", field->maxusage);
	for (j = 0; j < field->maxusage; j++) {
		tab(n+2, f); hid_resolv_usage(field->usage[j].hid, f); seq_printf(f, "\n");
	}
	if (field->logical_minimum != field->logical_maximum) {
		tab(n, f); seq_printf(f, "Logical Minimum(%d)\n", field->logical_minimum);
		tab(n, f); seq_printf(f, "Logical Maximum(%d)\n", field->logical_maximum);
	}
	if (field->physical_minimum != field->physical_maximum) {
		tab(n, f); seq_printf(f, "Physical Minimum(%d)\n", field->physical_minimum);
		tab(n, f); seq_printf(f, "Physical Maximum(%d)\n", field->physical_maximum);
	}
	if (field->unit_exponent) {
		tab(n, f); seq_printf(f, "Unit Exponent(%d)\n", field->unit_exponent);
	}
	if (field->unit) {
		static const char *systems[5] = { "None", "SI Linear", "SI Rotation", "English Linear", "English Rotation" };
		static const char *units[5][8] = {
			{ "None", "None", "None", "None", "None", "None", "None", "None" },
			{ "None", "Centimeter", "Gram", "Seconds", "Kelvin",     "Ampere", "Candela", "None" },
			{ "None", "Radians",    "Gram", "Seconds", "Kelvin",     "Ampere", "Candela", "None" },
			{ "None", "Inch",       "Slug", "Seconds", "Fahrenheit", "Ampere", "Candela", "None" },
			{ "None", "Degrees",    "Slug", "Seconds", "Fahrenheit", "Ampere", "Candela", "None" }
		};

		int i;
		int sys;
                __u32 data = field->unit;

		/* First nibble tells us which system we're in. */
		sys = data & 0xf;
		data >>= 4;

		if(sys > 4) {
			tab(n, f); seq_printf(f, "Unit(Invalid)\n");
		}
		else {
			int earlier_unit = 0;

			tab(n, f); seq_printf(f, "Unit(%s : ", systems[sys]);

			for (i=1 ; i<sizeof(__u32)*2 ; i++) {
				char nibble = data & 0xf;
				data >>= 4;
				if (nibble != 0) {
					if(earlier_unit++ > 0)
						seq_printf(f, "*");
					seq_printf(f, "%s", units[sys][i]);
					if(nibble != 1) {
						/* This is a _signed_ nibble(!) */

						int val = nibble & 0x7;
						if(nibble & 0x08)
							val = -((0x7 & ~val) +1);
						seq_printf(f, "^%d", val);
					}
				}
			}
			seq_printf(f, ")\n");
		}
	}
	tab(n, f); seq_printf(f, "Report Size(%u)\n", field->report_size);
	tab(n, f); seq_printf(f, "Report Count(%u)\n", field->report_count);
	tab(n, f); seq_printf(f, "Report Offset(%u)\n", field->report_offset);

	tab(n, f); seq_printf(f, "Flags( ");
	j = field->flags;
	seq_printf(f, "%s", HID_MAIN_ITEM_CONSTANT & j ? "Constant " : "");
	seq_printf(f, "%s", HID_MAIN_ITEM_VARIABLE & j ? "Variable " : "Array ");
	seq_printf(f, "%s", HID_MAIN_ITEM_RELATIVE & j ? "Relative " : "Absolute ");
	seq_printf(f, "%s", HID_MAIN_ITEM_WRAP & j ? "Wrap " : "");
	seq_printf(f, "%s", HID_MAIN_ITEM_NONLINEAR & j ? "NonLinear " : "");
	seq_printf(f, "%s", HID_MAIN_ITEM_NO_PREFERRED & j ? "NoPreferredState " : "");
	seq_printf(f, "%s", HID_MAIN_ITEM_NULL_STATE & j ? "NullState " : "");
	seq_printf(f, "%s", HID_MAIN_ITEM_VOLATILE & j ? "Volatile " : "");
	seq_printf(f, "%s", HID_MAIN_ITEM_BUFFERED_BYTE & j ? "BufferedByte " : "");
	seq_printf(f, ")\n");
}