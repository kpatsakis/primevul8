mm_append_debug(Buffer *m)
{
	if (auth_debug_init && buffer_len(&auth_debug)) {
		debug3("%s: Appending debug messages for child", __func__);
		buffer_append(m, buffer_ptr(&auth_debug),
		    buffer_len(&auth_debug));
		buffer_clear(&auth_debug);
	}
}