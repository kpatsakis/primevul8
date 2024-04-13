report_type_length_mismatch(proto_tree *tree, const gchar *descr, int length, gboolean is_error)
{
	if (is_error) {
		expert_add_info_format(NULL, tree, &ei_type_length_mismatch_error, "Trying to fetch %s with length %d", descr, length);
	} else {
		expert_add_info_format(NULL, tree, &ei_type_length_mismatch_warn, "Trying to fetch %s with length %d", descr, length);
	}

	if (is_error) {
		THROW(ReportedBoundsError);
	}
}