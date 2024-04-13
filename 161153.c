isofile_init_entry_data_file_list(struct iso9660 *iso9660)
{
	iso9660->data_file_list.first = NULL;
	iso9660->data_file_list.last = &(iso9660->data_file_list.first);
}