ccbaGenerateGlobalLocs(CCBORDA  *ccba)
{
l_int32  ncc, nb, n, i, j, k, xul, yul, x, y;
CCBORD  *ccb;
PTAA    *ptaal, *ptaag;
PTA     *ptal, *ptag;

    PROCNAME("ccbaGenerateGlobalLocs");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    ncc = ccbaGetCount(ccba);  /* number of c.c. */
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);

            /* Get the UL corner in global coords, (xul, yul), of the c.c. */
        boxaGetBoxGeometry(ccb->boxa, 0, &xul, &yul, NULL, NULL);

            /* Make a new global ptaa, removing any old one */
        ptaal = ccb->local;
        nb = ptaaGetCount(ptaal);   /* number of borders */
        if (ccb->global)   /* remove old one */
            ptaaDestroy(&ccb->global);
        if ((ptaag = ptaaCreate(nb)) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("ptaag not made", procName, 1);
        }
        ccb->global = ptaag;  /* save new one */

            /* Iterate through the borders for this c.c. */
        for (j = 0; j < nb; j++) {
            ptal = ptaaGetPta(ptaal, j, L_CLONE);
            n = ptaGetCount(ptal);   /* number of pixels in border */
            ptag = ptaCreate(n);
            ptaaAddPta(ptaag, ptag, L_INSERT);
            for (k = 0; k < n; k++) {
                ptaGetIPt(ptal, k, &x, &y);
                ptaAddPt(ptag, x  + xul, y + yul);
            }
            ptaDestroy(&ptal);
        }
        ccbDestroy(&ccb);
    }

    return 0;
}