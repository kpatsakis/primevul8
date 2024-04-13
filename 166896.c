ptvcursor_new(proto_tree *tree, tvbuff_t *tvb, gint offset)
{
	ptvcursor_t *ptvc;

	ptvc                    = wmem_new(wmem_packet_scope(), ptvcursor_t);
	ptvc->tree              = tree;
	ptvc->tvb               = tvb;
	ptvc->offset            = offset;
	ptvc->pushed_tree       = NULL;
	ptvc->pushed_tree_max   = 0;
	ptvc->pushed_tree_index = 0;
	return ptvc;
}