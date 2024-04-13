void _xml_endElementHandler(void *userData, const XML_Char *name)
{
	xml_parser *parser = (xml_parser *)userData;
	zend_string *tag_name;

	if (parser) {
		zval retval, args[2];

		tag_name = _xml_decode_tag(parser, (const char *)name);

		if (!Z_ISUNDEF(parser->endElementHandler)) {
			ZVAL_COPY(&args[0], &parser->index);
			ZVAL_STRING(&args[1], ZSTR_VAL(tag_name) + parser->toffset);

			xml_call_handler(parser, &parser->endElementHandler, parser->endElementPtr, 2, args, &retval);
			zval_ptr_dtor(&retval);
		} 

		if (!Z_ISUNDEF(parser->data)) {
			zval tag;

			if (parser->lastwasopen) {
				add_assoc_string(parser->ctag, "type", "complete");
			} else {
				array_init(&tag);
				  
				_xml_add_to_info(parser, ZSTR_VAL(tag_name) + parser->toffset);

				add_assoc_string(&tag, "tag", ZSTR_VAL(tag_name) + parser->toffset); /* cast to avoid gcc-warning */
				add_assoc_string(&tag, "type", "close");
				add_assoc_long(&tag, "level", parser->level);
				  
				zend_hash_next_index_insert(Z_ARRVAL(parser->data), &tag);
			}

			parser->lastwasopen = 0;
		}

		zend_string_release(tag_name);

		if ((parser->ltags) && (parser->level <= XML_MAXLEVEL)) {
			efree(parser->ltags[parser->level-1]);
		}

		parser->level--;
	}
}