isoent_cmp_node_joliet(const struct archive_rb_node *n1,
    const struct archive_rb_node *n2)
{
	const struct idrent *e1 = (const struct idrent *)n1;
	const struct idrent *e2 = (const struct idrent *)n2;

	return (isoent_cmp_joliet_identifier(e2->isoent, e1->isoent));
}