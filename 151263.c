int add_list_item(struct sa_item **list, char *item_name, int max_len)
{
	struct sa_item *e;
	int len;

	if ((len = strnlen(item_name, max_len)) == max_len)
		/* Item too long */
		return 0;

	while (*list != NULL) {
		e = *list;
		if (!strcmp(e->item_name, item_name))
			return 0;	/* Item found in list */
		list = &(e->next);
	}

	/* Item not found: Add it to the list */
	SREALLOC(*list, struct sa_item, sizeof(struct sa_item));
	e = *list;
	if ((e->item_name = (char *) malloc(len + 1)) == NULL) {
		perror("malloc");
		exit(4);
	}
	strcpy(e->item_name, item_name);

	return 1;
}