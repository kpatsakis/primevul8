proto_item_get_parent_nth(proto_item *ti, int gen) {
	if (!ti)
		return NULL;
	while (gen--) {
		ti = ti->parent;
		if (!ti)
			return NULL;
	}
	return ti;
}