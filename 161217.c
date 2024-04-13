isoent_cmp_node(const struct archive_rb_node *n1,
    const struct archive_rb_node *n2)
{
	const struct isoent *e1 = (const struct isoent *)n1;
	const struct isoent *e2 = (const struct isoent *)n2;

	return (strcmp(e1->file->basename.s, e2->file->basename.s));
}