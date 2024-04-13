win_size_save(garray_T *gap)

{
    win_T	*wp;

    ga_init2(gap, (int)sizeof(int), 1);
    if (ga_grow(gap, win_count() * 2) == OK)
	FOR_ALL_WINDOWS(wp)
	{
	    ((int *)gap->ga_data)[gap->ga_len++] =
					       wp->w_width + wp->w_vsep_width;
	    ((int *)gap->ga_data)[gap->ga_len++] = wp->w_height;
	}
}