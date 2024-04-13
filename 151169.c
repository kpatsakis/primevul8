ccbaDisplaySPBorder(CCBORDA  *ccba)
{
l_int32  ncc, npt, i, j, x, y;
CCBORD  *ccb;
PIX     *pixd;
PTA     *ptag;

    PROCNAME("ccbaDisplaySPBorder");

    if (!ccba)
        return (PIX *)ERROR_PTR("ccba not defined", procName, NULL);

    if ((pixd = pixCreate(ccba->w, ccba->h, 1)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    ncc = ccbaGetCount(ccba);   /* number of c.c. */
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);
        if ((ptag = ccb->spglobal) == NULL) {
            L_WARNING("spglobal pixel loc array not found\n", procName);
            ccbDestroy(&ccb);
            continue;
        }
        npt = ptaGetCount(ptag);   /* number of pixels on path */
        for (j = 0; j < npt; j++) {
            ptaGetIPt(ptag, j, &x, &y);
            pixSetPixel(pixd, x, y, 1);
        }
        ccbDestroy(&ccb);  /* clone ref */
    }

    return pixd;
}