initialize_prefix(gpointer k, gpointer v, gpointer u _U_) {
	((prefix_initializer_t)v)((const char *)k);
	return TRUE;
}