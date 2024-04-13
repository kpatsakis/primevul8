YCbCr_tables_init (void)
{
	g_mutex_lock (&Tables_Mutex);

	if (YCbCr_R_Cr_Tab == NULL) {
		int i, v;

		YCbCr_R_Cr_Tab = g_new (int, 256);
		YCbCr_G_Cb_Tab = g_new (int, 256);
		YCbCr_G_Cr_Tab = g_new (int, 256);
		YCbCr_B_Cb_Tab = g_new (int, 256);

		for (i = 0, v = -128; i <= 255; i++, v++) {
			YCbCr_R_Cr_Tab[i] = SCALE_DOWN (SCALE_UP (1.402) * v + ONE_HALF);
			YCbCr_G_Cb_Tab[i] = - SCALE_UP (0.34414) * v;
			YCbCr_G_Cr_Tab[i] = - SCALE_UP (0.71414) * v + ONE_HALF;
			YCbCr_B_Cb_Tab[i] = SCALE_DOWN (SCALE_UP (1.77200) * v + ONE_HALF);
		}
	}

	g_mutex_unlock (&Tables_Mutex);
}