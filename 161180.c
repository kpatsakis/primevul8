isofile_add_entry(struct iso9660 *iso9660, struct isofile *file)
{
	file->allnext = NULL;
	*iso9660->all_file_list.last = file;
	iso9660->all_file_list.last = &(file->allnext);
}