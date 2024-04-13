isofile_free_all_entries(struct iso9660 *iso9660)
{
	struct isofile *file, *file_next;

	file = iso9660->all_file_list.first;
	while (file != NULL) {
		file_next = file->allnext;
		isofile_free(file);
		file = file_next;
	}
}