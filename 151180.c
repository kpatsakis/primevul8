ccbaDestroy(CCBORDA  **pccba)
{
l_int32   i;
CCBORDA  *ccba;

    PROCNAME("ccbaDestroy");

    if (pccba == NULL) {
        L_WARNING("ptr address is NULL!\n", procName);
        return;
    }

    if ((ccba = *pccba) == NULL)
        return;

    pixDestroy(&ccba->pix);
    for (i = 0; i < ccba->n; i++)
        ccbDestroy(&ccba->ccb[i]);
    LEPT_FREE(ccba->ccb);
    LEPT_FREE(ccba);
    *pccba = NULL;
}