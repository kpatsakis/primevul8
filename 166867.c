proto_tree_add_bitmask_text(proto_tree *parent_tree, tvbuff_t *tvb,
			    const guint offset, const guint len,
			    const char *name, const char *fallback,
			    const gint ett, int * const *fields,
			    const guint encoding, const int flags)
{
	proto_item *item = NULL;
	guint64     value;

	if (parent_tree) {
		item = proto_tree_add_text_internal(parent_tree, tvb, offset, len, "%s", name ? name : "");
		value = get_uint64_value(parent_tree, tvb, offset, len, encoding);
		if (proto_item_add_bitmask_tree(item, tvb, offset, len, ett, fields,
		    flags, TRUE, FALSE, NULL, value) && fallback) {
			/* Still at first item - append 'fallback' text if any */
			proto_item_append_text(item, "%s", fallback);
		}
	}

	return item;
}