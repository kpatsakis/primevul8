SPL_METHOD(SplHeap, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	/* do nothing, the iterator always points to the top element */
}