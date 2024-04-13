ccbaAddCcb(CCBORDA  *ccba,
           CCBORD   *ccb)
{
l_int32  n;

    PROCNAME("ccbaAddCcb");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);
    if (!ccb)
        return ERROR_INT("ccb not defined", procName, 1);

    n = ccbaGetCount(ccba);
    if (n >= ccba->nalloc)
        ccbaExtendArray(ccba);
    ccba->ccb[n] = ccb;
    ccba->n++;
    return 0;
}