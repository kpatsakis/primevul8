idr_resolve(struct idr *idr, void (*fsetnum)(unsigned char *p, int num))
{
	struct idrent *n;
	unsigned char *p;

	for (n = idr->wait_list.first; n != NULL; n = n->wnext) {
		idr_extend_identifier(n, idr->num_size, idr->null_size);
		p = (unsigned char *)n->isoent->identifier + n->noff;
		do {
			fsetnum(p, n->avail->rename_num++);
		} while (!__archive_rb_tree_insert_node(
		    &(idr->rbtree), &(n->rbnode)));
	}
}