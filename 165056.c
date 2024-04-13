void _af_print_pvlist (AUpvlist list)
{
	int i;

	assert(list);

	printf("list.valid: %d\n", list->valid);
	printf("list.count: %d\n", list->count);

	for (i=0; i<list->count; i++)
	{
		printf("item %d valid %d, should be %d\n",
			i, list->items[i].valid, _AU_VALID_PVITEM);

		switch (list->items[i].type)
		{
			case AU_PVTYPE_LONG:
				printf("item #%d, parameter %d, long: %ld\n",
					i, list->items[i].parameter,
					list->items[i].value.l);
				break;
			case AU_PVTYPE_DOUBLE:
				printf("item #%d, parameter %d, double: %f\n",
					i, list->items[i].parameter,
					list->items[i].value.d);
				break;
			case AU_PVTYPE_PTR:
				printf("item #%d, parameter %d, pointer: %p\n",
					i, list->items[i].parameter,
					list->items[i].value.v);
				break;

			default:
				printf("item #%d, invalid type %d\n", i,
					list->items[i].type);
				assert(0);
				break;
		}
	}
}