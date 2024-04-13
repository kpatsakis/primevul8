isofile_init_entry_list(struct iso9660 *iso9660)
{
	iso9660->all_file_list.first = NULL;
	iso9660->all_file_list.last = &(iso9660->all_file_list.first);
}