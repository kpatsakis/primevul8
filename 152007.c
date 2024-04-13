static void xml_set_handler(zval *handler, zval *data)
{
	/* If we have already a handler, release it */
	if (handler) {
		zval_ptr_dtor(handler);
	}

	/* IS_ARRAY might indicate that we're using array($obj, 'method') syntax */
	if (Z_TYPE_P(data) != IS_ARRAY && Z_TYPE_P(data) != IS_OBJECT) {
		convert_to_string_ex(data);
		if (Z_STRLEN_P(data) == 0) {
			ZVAL_UNDEF(handler);
			return;
		}
	}

	ZVAL_COPY(handler, data);
}