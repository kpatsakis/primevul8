pixGetAllCCBorders(PIX  *pixs)
{
l_int32   n, i;
BOX      *box;
BOXA     *boxa;
CCBORDA  *ccba;
CCBORD   *ccb;
PIX      *pix;
PIXA     *pixa;

    PROCNAME("pixGetAllCCBorders");

    if (!pixs)
        return (CCBORDA *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 1)
        return (CCBORDA *)ERROR_PTR("pixs not binary", procName, NULL);

    if ((boxa = pixConnComp(pixs, &pixa, 8)) == NULL)
        return (CCBORDA *)ERROR_PTR("boxa not made", procName, NULL);
    n = boxaGetCount(boxa);

    if ((ccba = ccbaCreate(pixs, n)) == NULL) {
        boxaDestroy(&boxa);
        pixaDestroy(&pixa);
        return (CCBORDA *)ERROR_PTR("ccba not made", procName, NULL);
    }
    for (i = 0; i < n; i++) {
        if ((pix = pixaGetPix(pixa, i, L_CLONE)) == NULL) {
            ccbaDestroy(&ccba);
            pixaDestroy(&pixa);
            boxaDestroy(&boxa);
            return (CCBORDA *)ERROR_PTR("pix not found", procName, NULL);
        }
        if ((box = pixaGetBox(pixa, i, L_CLONE)) == NULL) {
            ccbaDestroy(&ccba);
            pixaDestroy(&pixa);
            boxaDestroy(&boxa);
            pixDestroy(&pix);
            return (CCBORDA *)ERROR_PTR("box not found", procName, NULL);
        }
        ccb = pixGetCCBorders(pix, box);
        pixDestroy(&pix);
        boxDestroy(&box);
        if (!ccb) {
            ccbaDestroy(&ccba);
            pixaDestroy(&pixa);
            boxaDestroy(&boxa);
            return (CCBORDA *)ERROR_PTR("ccb not made", procName, NULL);
        }
/*        ptaWriteStream(stderr, ccb->local, 1); */
        ccbaAddCcb(ccba, ccb);
    }

    boxaDestroy(&boxa);
    pixaDestroy(&pixa);
    return ccba;
}