
static int io_queue_rsrc_removal(struct io_rsrc_data *data, unsigned idx,
				 struct io_rsrc_node *node, void *rsrc)
{
	u64 *tag_slot = io_get_tag_slot(data, idx);
	struct io_rsrc_put *prsrc;

	prsrc = kzalloc(sizeof(*prsrc), GFP_KERNEL);
	if (!prsrc)
		return -ENOMEM;

	prsrc->tag = *tag_slot;
	*tag_slot = 0;
	prsrc->rsrc = rsrc;
	list_add(&prsrc->list, &node->rsrc_list);
	return 0;