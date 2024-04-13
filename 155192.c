CMYK_table_init (void)
{
	g_mutex_lock (&Tables_Mutex);

	if (CMYK_Tab == NULL) {
		int    v, k, i;
		double k1;

		/* tab[k * 256 + v] = v * k / 255.0 */

		CMYK_Tab = g_new (unsigned char, 256 * 256);
		i = 0;
		for (k = 0; k <= 255; k++) {
			k1 = (double) k / 255.0;
			for (v = 0; v <= 255; v++)
				CMYK_Tab[i++] = (double) v * k1;
		}
	}

	g_mutex_unlock (&Tables_Mutex);
}