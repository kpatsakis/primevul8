static void _xml_add_to_info(xml_parser *parser,char *name)
{
	zval *element;

	if (Z_ISUNDEF(parser->info)) {
		return;
	}

	if ((element = zend_hash_str_find(Z_ARRVAL(parser->info), name, strlen(name))) == NULL) {
		zval values;
		array_init(&values);
		element = zend_hash_str_update(Z_ARRVAL(parser->info), name, strlen(name), &values);
	} 
			
	add_next_index_long(element, parser->curtag);
	
	parser->curtag++;
}