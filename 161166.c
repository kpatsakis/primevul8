iso9660_free(struct archive_write *a)
{
	struct iso9660 *iso9660;
	int i, ret;

	iso9660 = a->format_data;

	/* Close the temporary file. */
	if (iso9660->temp_fd >= 0)
		close(iso9660->temp_fd);

	/* Free some stuff for zisofs operations. */
	ret = zisofs_free(a);

	/* Remove directory entries in tree which includes file entries. */
	isoent_free_all(iso9660->primary.rootent);
	for (i = 0; i < iso9660->primary.max_depth; i++)
		free(iso9660->primary.pathtbl[i].sorted);
	free(iso9660->primary.pathtbl);

	if (iso9660->opt.joliet) {
		isoent_free_all(iso9660->joliet.rootent);
		for (i = 0; i < iso9660->joliet.max_depth; i++)
			free(iso9660->joliet.pathtbl[i].sorted);
		free(iso9660->joliet.pathtbl);
	}

	/* Remove isofile entries. */
	isofile_free_all_entries(iso9660);
	isofile_free_hardlinks(iso9660);

	archive_string_free(&(iso9660->cur_dirstr));
	archive_string_free(&(iso9660->volume_identifier));
	archive_string_free(&(iso9660->publisher_identifier));
	archive_string_free(&(iso9660->data_preparer_identifier));
	archive_string_free(&(iso9660->application_identifier));
	archive_string_free(&(iso9660->copyright_file_identifier));
	archive_string_free(&(iso9660->abstract_file_identifier));
	archive_string_free(&(iso9660->bibliographic_file_identifier));
	archive_string_free(&(iso9660->el_torito.catalog_filename));
	archive_string_free(&(iso9660->el_torito.boot_filename));
	archive_string_free(&(iso9660->el_torito.id));
	archive_string_free(&(iso9660->utf16be));
	archive_string_free(&(iso9660->mbs));

	free(iso9660);
	a->format_data = NULL;

	return (ret);
}