ccbaGenerateStepChains(CCBORDA  *ccba)
{
l_int32  ncc, nb, n, i, j, k;
l_int32  px, py, cx, cy, stepdir;
l_int32  dirtab[][3] = {{1, 2, 3}, {0, -1, 4}, {7, 6, 5}};
CCBORD  *ccb;
NUMA    *na;
NUMAA   *naa;   /* step chain code; to be made */
PTA     *ptal;
PTAA    *ptaal;  /* local chain code */

    PROCNAME("ccbaGenerateStepChains");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    ncc = ccbaGetCount(ccba);  /* number of c.c. */
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);

            /* Make a new step numaa, removing any old one */
        ptaal = ccb->local;
        nb = ptaaGetCount(ptaal);  /* number of borders */
        if (ccb->step)  /* remove old one */
            numaaDestroy(&ccb->step);
        if ((naa = numaaCreate(nb)) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("naa not made", procName, 1);
        }
        ccb->step = naa;  /* save new one */

            /* Iterate through the borders for this c.c. */
        for (j = 0; j < nb; j++) {
            ptal = ptaaGetPta(ptaal, j, L_CLONE);
            n = ptaGetCount(ptal);   /* number of pixels in border */
            if (n == 1) {  /* isolated pixel */
                na = numaCreate(1);   /* but leave it empty */
            } else {   /* trace out the boundary */
                na = numaCreate(n);
                ptaGetIPt(ptal, 0, &px, &py);
                for (k = 1; k < n; k++) {
                    ptaGetIPt(ptal, k, &cx, &cy);
                    stepdir = dirtab[1 + cy - py][1 + cx - px];
                    numaAddNumber(na, stepdir);
                    px = cx;
                    py = cy;
                }
            }
            numaaAddNuma(naa, na, L_INSERT);
            ptaDestroy(&ptal);
        }
        ccbDestroy(&ccb);  /* just decrement refcount */
    }

    return 0;
}