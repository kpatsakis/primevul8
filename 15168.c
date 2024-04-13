eval_init(void)
{
    evalvars_init();
    func_init();

#ifdef EBCDIC
    /*
     * Sort the function table, to enable binary search.
     */
    sortFunctions();
#endif
}