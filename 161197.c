isofile_new(struct archive_write *a, struct archive_entry *entry)
{
	struct isofile *file;

	file = calloc(1, sizeof(*file));
	if (file == NULL)
		return (NULL);

	if (entry != NULL)
		file->entry = archive_entry_clone(entry);
	else
		file->entry = archive_entry_new2(&a->archive);
	if (file->entry == NULL) {
		free(file);
		return (NULL);
	}
	archive_string_init(&(file->parentdir));
	archive_string_init(&(file->basename));
	archive_string_init(&(file->basename_utf16));
	archive_string_init(&(file->symlink));
	file->cur_content = &(file->content);

	return (file);
}