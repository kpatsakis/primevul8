prefix_equal (gconstpointer ap, gconstpointer bp) {
	const gchar* a = (const gchar *)ap;
	const gchar* b = (const gchar *)bp;

	do {
		gchar ac = *a++;
		gchar bc = *b++;

		if ( (ac == '.' || ac == '\0') &&   (bc == '.' || bc == '\0') ) return TRUE;

		if ( (ac == '.' || ac == '\0') && ! (bc == '.' || bc == '\0') ) return FALSE;
		if ( (bc == '.' || bc == '\0') && ! (ac == '.' || ac == '\0') ) return FALSE;

		if (ac != bc) return FALSE;
	} while (1);

	return FALSE;
}