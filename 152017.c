void php_filter_number_float(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* strip everything [^0-9+-] */
	const unsigned char allowed_list[] = "+-" DIGIT;
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);

	/* depending on flags, strip '.', 'e', ",", "'" */
	if (flags & FILTER_FLAG_ALLOW_FRACTION) {
		filter_map_update(&map, 2, (const unsigned char *) ".");
	}
	if (flags & FILTER_FLAG_ALLOW_THOUSAND) {
		filter_map_update(&map, 3,  (const unsigned char *) ",");
	}
	if (flags & FILTER_FLAG_ALLOW_SCIENTIFIC) {
		filter_map_update(&map, 4,  (const unsigned char *) "eE");
	}
	filter_map_apply(value, &map);
}