void _xml_startElementHandler(void *userData, const XML_Char *name, const XML_Char **attributes)
{
	xml_parser *parser = (xml_parser *)userData;
	const char **attrs = (const char **) attributes;
	zend_string *att, *tag_name, *val;
	zval retval, args[3];

	if (parser) {
		parser->level++;

		tag_name = _xml_decode_tag(parser, (const char *)name);

		if (!Z_ISUNDEF(parser->startElementHandler)) {
			ZVAL_COPY(&args[0], &parser->index);
			ZVAL_STRING(&args[1], ZSTR_VAL(tag_name) + parser->toffset);
			array_init(&args[2]);

			while (attributes && *attributes) {
				zval tmp;

				att = _xml_decode_tag(parser, (const char *)attributes[0]);
				val = xml_utf8_decode(attributes[1], strlen((char *)attributes[1]), parser->target_encoding);

				ZVAL_STR(&tmp, val);
				zend_symtable_update(Z_ARRVAL(args[2]), att, &tmp);

				attributes += 2;

				zend_string_release(att);
			}
			
			xml_call_handler(parser, &parser->startElementHandler, parser->startElementPtr, 3, args, &retval);
			zval_ptr_dtor(&retval);
		} 

		if (!Z_ISUNDEF(parser->data)) {
			if (parser->level <= XML_MAXLEVEL)  {
				zval tag, atr;
				int atcnt = 0;

				array_init(&tag);
				array_init(&atr);

				_xml_add_to_info(parser, ZSTR_VAL(tag_name) + parser->toffset);

				add_assoc_string(&tag, "tag", ZSTR_VAL(tag_name) + parser->toffset); /* cast to avoid gcc-warning */
				add_assoc_string(&tag, "type", "open");
				add_assoc_long(&tag, "level", parser->level);

				parser->ltags[parser->level-1] = estrdup(ZSTR_VAL(tag_name));
				parser->lastwasopen = 1;

				attributes = (const XML_Char **) attrs;

				while (attributes && *attributes) {
					zval tmp;

					att = _xml_decode_tag(parser, (const char *)attributes[0]);
					val = xml_utf8_decode(attributes[1], strlen((char *)attributes[1]), parser->target_encoding);

					ZVAL_STR(&tmp, val);
					zend_symtable_update(Z_ARRVAL(atr), att, &tmp);

					atcnt++;
					attributes += 2;

					zend_string_release(att);
				}

				if (atcnt) {
					zend_hash_str_add(Z_ARRVAL(tag), "attributes", sizeof("attributes") - 1, &atr);
				} else {
					zval_ptr_dtor(&atr);
				}

				parser->ctag = zend_hash_next_index_insert(Z_ARRVAL(parser->data), &tag);
			} else if (parser->level == (XML_MAXLEVEL + 1)) {
							php_error_docref(NULL, E_WARNING, "Maximum depth exceeded - Results truncated");
			}
		}

		zend_string_release(tag_name);
	}
}