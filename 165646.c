sg_proc_init(void)
{
	struct proc_dir_entry *p;

	p = proc_mkdir("scsi/sg", NULL);
	if (!p)
		return 1;

	proc_create("allow_dio", S_IRUGO | S_IWUSR, p, &adio_proc_ops);
	proc_create_seq("debug", S_IRUGO, p, &debug_seq_ops);
	proc_create("def_reserved_size", S_IRUGO | S_IWUSR, p, &dressz_proc_ops);
	proc_create_single("device_hdr", S_IRUGO, p, sg_proc_seq_show_devhdr);
	proc_create_seq("devices", S_IRUGO, p, &dev_seq_ops);
	proc_create_seq("device_strs", S_IRUGO, p, &devstrs_seq_ops);
	proc_create_single("version", S_IRUGO, p, sg_proc_seq_show_version);
	return 0;
}