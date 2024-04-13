set_ref_in_tagfunc(int copyID UNUSED)
{
    int	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    return abort;
}