static void put_in_order(struct List *newitem, struct StatList *list,
			 int (*cmpfn)(struct List *, struct List *))
{
	int res;
	struct List *item;

	statlist_for_each(item, list) {
		res = cmpfn(item, newitem);
		if (res == 0)
			fatal("put_in_order: found existing elem");
		else if (res > 0) {
			statlist_put_before(list, newitem, item);
			return;
		}
	}
	statlist_append(list, newitem);
}