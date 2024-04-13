static bool is_valid_ordinal_table_size(ut64 size) {
	return size > 0 && size <= UT16_MAX;
}