set_tagfunc_option(void)
{
#ifdef FEAT_EVAL
    free_callback(&tfu_cb);
    free_callback(&curbuf->b_tfu_cb);

    if (*curbuf->b_p_tfu == NUL)
	return OK;

    if (option_set_callback_func(curbuf->b_p_tfu, &tfu_cb) == FAIL)
	return FAIL;

    copy_callback(&curbuf->b_tfu_cb, &tfu_cb);
#endif

    return OK;
}