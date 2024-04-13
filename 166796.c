proto_item_get_parent(const proto_item *ti) {
	if (!ti)
		return NULL;
	return ti->parent;
}