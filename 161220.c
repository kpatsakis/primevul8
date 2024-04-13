isoent_cmp_key_joliet(const struct archive_rb_node *node, const void *key)
{
	const struct isoent *isoent = (const struct isoent *)key;
	const struct idrent *idrent = (const struct idrent *)node;

	return (isoent_cmp_joliet_identifier(isoent, idrent->isoent));
}