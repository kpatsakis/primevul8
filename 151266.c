int search_list_item(struct sa_item *list, char *item_name)
{
	while (list != NULL) {
		if (!strcmp(list->item_name, item_name))
			return 1;	/* Item found in list */
		list = list->next;
	}

	/* Item not found */
	return 0;
}