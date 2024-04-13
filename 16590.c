static pyc_object *get_tuple_object(RzBinPycObj *pyc, RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut32 n = 0;

	n = get_ut32(buffer, &error);
	if (n > ST32_MAX) {
		RZ_LOG_ERROR("bad marshal data (tuple size out of range)\n");
		return NULL;
	}
	if (error) {
		return NULL;
	}
	ret = get_array_object_generic(pyc, buffer, n);
	if (ret) {
		ret->type = TYPE_TUPLE;
		return ret;
	}
	return NULL;
}