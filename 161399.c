static int phar_compare_dir_name(const void *a, const void *b TSRMLS_DC)  /* {{{ */
{
	Bucket *f;
	Bucket *s;
	int result;

	f = *((Bucket **) a);
	s = *((Bucket **) b);
	result = zend_binary_strcmp(f->arKey, f->nKeyLength, s->arKey, s->nKeyLength);

	if (result < 0) {
		return -1;
	} else if (result > 0) {
		return 1;
	} else {
		return 0;
	}
}