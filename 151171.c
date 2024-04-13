ccbaExtendArray(CCBORDA  *ccba)
{
    PROCNAME("ccbaExtendArray");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    if ((ccba->ccb = (CCBORD **)reallocNew((void **)&ccba->ccb,
                                sizeof(CCBORD *) * ccba->nalloc,
                                2 * sizeof(CCBORD *) * ccba->nalloc)) == NULL)
        return ERROR_INT("new ptr array not returned", procName, 1);

    ccba->nalloc = 2 * ccba->nalloc;
    return 0;
}