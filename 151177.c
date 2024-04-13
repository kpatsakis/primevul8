ccbaGetCount(CCBORDA  *ccba)
{

    PROCNAME("ccbaGetCount");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 0);

    return ccba->n;
}