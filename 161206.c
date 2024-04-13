isoent_clone_tree(struct archive_write *a, struct isoent **nroot,
    struct isoent *root)
{
	struct isoent *np, *xroot, *newent;

	np = root;
	xroot = NULL;
	do {
		newent = isoent_clone(np);
		if (newent == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory");
			return (ARCHIVE_FATAL);
		}
		if (xroot == NULL) {
			*nroot = xroot = newent;
			newent->parent = xroot;
		} else
			isoent_add_child_tail(xroot, newent);
		if (np->dir && np->children.first != NULL) {
			/* Enter to sub directories. */
			np = np->children.first;
			xroot = newent;
			continue;
		}
		while (np != np->parent) {
			if (np->chnext == NULL) {
				/* Return to the parent directory. */
				np = np->parent;
				xroot = xroot->parent;
			} else {
				np = np->chnext;
				break;
			}
		}
	} while (np != np->parent);

	return (ARCHIVE_OK);
}