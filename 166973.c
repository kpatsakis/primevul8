proto_tree_set_string(field_info *fi, const char* value)
{
	if (value) {
		fvalue_set_string(&fi->value, value);
	} else {
		/*
		 * XXX - why is a null value for a string field
		 * considered valid?
		 */
		fvalue_set_string(&fi->value, "[ Null ]");
	}
}