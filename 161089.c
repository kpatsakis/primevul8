idr_cleanup(struct idr *idr)
{
	free(idr->idrent_pool);
}