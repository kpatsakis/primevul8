ccbaDisplayImage2(CCBORDA  *ccba)
{
l_int32  ncc, nb, n, i, j, k, x, y, xul, yul, w, h;
l_int32  fpx, fpy, spx, spy, xs, ys;
BOXA    *boxa;
CCBORD  *ccb;
PIX     *pixd, *pixc, *pixs;
PTAA    *ptaa;
PTA     *pta;

    PROCNAME("ccbaDisplayImage2");

    if (!ccba)
        return (PIX *)ERROR_PTR("ccba not defined", procName, NULL);

    if ((pixd = pixCreate(ccba->w, ccba->h, 1)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    ncc = ccbaGetCount(ccba);
    for (i = 0; i < ncc; i++) {
            /* Generate clipping mask from border pixels and seed image
             * from one seed for each closed border. */
        ccb = ccbaGetCcb(ccba, i);
        if ((boxa = ccb->boxa) == NULL) {
            pixDestroy(&pixd);
            ccbDestroy(&ccb);
            return (PIX *)ERROR_PTR("boxa not found", procName, NULL);
        }
        if (boxaGetBoxGeometry(boxa, 0, &xul, &yul, &w, &h)) {
            pixDestroy(&pixd);
            ccbDestroy(&ccb);
            return (PIX *)ERROR_PTR("b. box not found", procName, NULL);
        }
        pixc = pixCreate(w + 2, h + 2, 1);
        pixs = pixCreateTemplate(pixc);

        if ((ptaa = ccb->local) == NULL) {
            pixDestroy(&pixc);
            pixDestroy(&pixs);
            ccbDestroy(&ccb);
            L_WARNING("local chain array not found\n", procName);
            continue;
        }
        nb = ptaaGetCount(ptaa);   /* number of borders in the c.c.  */
        for (j = 0; j < nb; j++) {
            pta = ptaaGetPta(ptaa, j, L_CLONE);
            n = ptaGetCount(pta);   /* number of pixels in the border */

                /* Render border pixels in pixc */
            for (k = 0; k < n; k++) {
                ptaGetIPt(pta, k, &x, &y);
                pixSetPixel(pixc, x + 1, y + 1, 1);
                if (k == 0) {
                    fpx = x + 1;
                    fpy = y + 1;
                } else if (k == 1) {
                    spx = x + 1;
                    spy = y + 1;
                }
            }

                /* Get and set seed pixel for this border in pixs */
            if (n > 1)
                locateOutsideSeedPixel(fpx, fpy, spx, spy, &xs, &ys);
            else  /* isolated c.c. */
                xs = ys = 0;
            pixSetPixel(pixs, xs, ys, 1);
            ptaDestroy(&pta);
        }

            /* Fill from seeds in pixs, using pixc as the clipping mask,
             * to reconstruct the c.c. */
        pixInvert(pixc, pixc);  /* to convert clipping -> filling mask */
        pixSeedfillBinary(pixs, pixs, pixc, 4);  /* 4-fill */
        pixInvert(pixs, pixs);  /* to make the c.c. */

            /* XOR into the dest */
        pixRasterop(pixd, xul, yul, w, h, PIX_XOR, pixs, 1, 1);

        pixDestroy(&pixc);
        pixDestroy(&pixs);
        ccbDestroy(&ccb);  /* ref-counted */
    }
    return pixd;
}