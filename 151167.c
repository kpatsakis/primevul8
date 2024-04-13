ccbDestroy(CCBORD  **pccb)
{
CCBORD  *ccb;

    PROCNAME("ccbDestroy");

    if (pccb == NULL) {
        L_WARNING("ptr address is NULL!\n", procName);
        return;
    }

    if ((ccb = *pccb) == NULL)
        return;

    ccb->refcount--;
    if (ccb->refcount == 0) {
        if (ccb->pix)
            pixDestroy(&ccb->pix);
        if (ccb->boxa)
            boxaDestroy(&ccb->boxa);
        if (ccb->start)
            ptaDestroy(&ccb->start);
        if (ccb->local)
            ptaaDestroy(&ccb->local);
        if (ccb->global)
            ptaaDestroy(&ccb->global);
        if (ccb->step)
            numaaDestroy(&ccb->step);
        if (ccb->splocal)
            ptaDestroy(&ccb->splocal);
        if (ccb->spglobal)
            ptaDestroy(&ccb->spglobal);
        LEPT_FREE(ccb);
        *pccb = NULL;
    }
}