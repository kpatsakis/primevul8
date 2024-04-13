ptvcursor_add_no_advance(ptvcursor_t* ptvc, int hf, gint length,
			 const guint encoding)
{
	proto_item *item;

	item = proto_tree_add_item(ptvc->tree, hf, ptvc->tvb, ptvc->offset,
				   length, encoding);

	return item;
}