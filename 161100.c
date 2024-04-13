idr_register(struct idr *idr, struct isoent *isoent, int weight, int noff)
{
	struct idrent *idrent, *n;

	idrent = &(idr->idrent_pool[idr->pool_idx++]);
	idrent->wnext = idrent->avail = NULL;
	idrent->isoent = isoent;
	idrent->weight = weight;
	idrent->noff = noff;
	idrent->rename_num = 0;

	if (!__archive_rb_tree_insert_node(&(idr->rbtree), &(idrent->rbnode))) {
		n = (struct idrent *)__archive_rb_tree_find_node(
		    &(idr->rbtree), idrent->isoent);
		if (n != NULL) {
			/* this `idrent' needs to rename. */
			idrent->avail = n;
			*idr->wait_list.last = idrent;
			idr->wait_list.last = &(idrent->wnext);
		}
	}
}