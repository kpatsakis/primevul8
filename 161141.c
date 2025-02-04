isoent_create_virtual_dir(struct archive_write *a, struct iso9660 *iso9660, const char *pathname)
{
	struct isofile *file;
	struct isoent *isoent;

	file = isofile_new(a, NULL);
	if (file == NULL)
		return (NULL);
	archive_entry_set_pathname(file->entry, pathname);
	archive_entry_unset_mtime(file->entry);
	archive_entry_unset_atime(file->entry);
	archive_entry_unset_ctime(file->entry);
	archive_entry_set_uid(file->entry, getuid());
	archive_entry_set_gid(file->entry, getgid());
	archive_entry_set_mode(file->entry, 0555 | AE_IFDIR);
	archive_entry_set_nlink(file->entry, 2);
	if (isofile_gen_utility_names(a, file) < ARCHIVE_WARN) {
		isofile_free(file);
		return (NULL);
	}
	isofile_add_entry(iso9660, file);

	isoent = isoent_new(file);
	if (isoent == NULL)
		return (NULL);
	isoent->dir = 1;
	isoent->virtual = 1;

	return (isoent);
}