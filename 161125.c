isofile_free(struct isofile *file)
{
	struct content *con, *tmp;

	con = file->content.next;
	while (con != NULL) {
		tmp = con;
		con = con->next;
		free(tmp);
	}
	archive_entry_free(file->entry);
	archive_string_free(&(file->parentdir));
	archive_string_free(&(file->basename));
	archive_string_free(&(file->basename_utf16));
	archive_string_free(&(file->symlink));
	free(file);
}