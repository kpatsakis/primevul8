static int phar_add_empty(HashTable *ht, char *arKey, uint nKeyLength)  /* {{{ */
{
	void *dummy = (char *) 1;

	return zend_hash_update(ht, arKey, nKeyLength, (void *) &dummy, sizeof(void *), NULL);
}