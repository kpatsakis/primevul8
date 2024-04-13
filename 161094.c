isoent_cmp_key(const struct archive_rb_node *n, const void *key)
{
	const struct isoent *e = (const struct isoent *)n;

	return (strcmp(e->file->basename.s, (const char *)key));
}