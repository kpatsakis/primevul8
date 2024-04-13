static int hid_debug_rdesc_show(struct seq_file *f, void *p)
{
	struct hid_device *hdev = f->private;
	const __u8 *rdesc = hdev->rdesc;
	unsigned rsize = hdev->rsize;
	int i;

	if (!rdesc) {
		rdesc = hdev->dev_rdesc;
		rsize = hdev->dev_rsize;
	}

	/* dump HID report descriptor */
	for (i = 0; i < rsize; i++)
		seq_printf(f, "%02x ", rdesc[i]);
	seq_printf(f, "\n\n");

	/* dump parsed data and input mappings */
	hid_dump_device(hdev, f);
	seq_printf(f, "\n");
	hid_dump_input_mapping(hdev, f);

	return 0;
}