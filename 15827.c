free_tagfunc_option(void)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}