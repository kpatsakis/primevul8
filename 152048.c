void php_filter_email(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Check section 6 of rfc 822 http://www.faqs.org/rfcs/rfc822.html */
	const unsigned char allowed_list[] = LOWALPHA HIALPHA DIGIT "!#$%&'*+-=?^_`{|}~@.[]";
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);
	filter_map_apply(value, &map);
}