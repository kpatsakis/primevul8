void php_filter_number_int(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* strip everything [^0-9+-] */
	const unsigned char allowed_list[] = "+-" DIGIT;
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);
	filter_map_apply(value, &map);
}