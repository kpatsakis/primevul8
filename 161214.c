isofile_add_data_file(struct iso9660 *iso9660, struct isofile *file)
{
	file->datanext = NULL;
	*iso9660->data_file_list.last = file;
	iso9660->data_file_list.last = &(file->datanext);
}