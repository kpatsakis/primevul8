ccbaStepChainsToPixCoords(CCBORDA  *ccba,
                          l_int32   coordtype)
{
l_int32  ncc, nb, n, i, j, k;
l_int32  xul, yul, xstart, ystart, x, y, stepdir;
BOXA    *boxa;
CCBORD  *ccb;
NUMA    *na;
NUMAA   *naa;
PTAA    *ptaan;  /* new pix coord ptaa */
PTA     *ptas, *ptan;

    PROCNAME("ccbaStepChainsToPixCoords");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);
    if (coordtype != CCB_GLOBAL_COORDS && coordtype != CCB_LOCAL_COORDS)
        return ERROR_INT("coordtype not valid", procName, 1);

    ncc = ccbaGetCount(ccba);  /* number of c.c. */
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);
        if ((naa = ccb->step) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("step numaa not found", procName, 1);
        } if ((boxa = ccb->boxa) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("boxa not found", procName, 1);
        } if ((ptas = ccb->start) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("start pta not found", procName, 1);
        }

            /* For global coords, get the (xul, yul) of the c.c.;
             * otherwise, use relative coords. */
        if (coordtype == CCB_LOCAL_COORDS) {
            xul = 0;
            yul = 0;
        } else {  /* coordtype == CCB_GLOBAL_COORDS */
                /* Get UL corner in global coords */
            if (boxaGetBoxGeometry(boxa, 0, &xul, &yul, NULL, NULL)) {
                ccbDestroy(&ccb);
                return ERROR_INT("bounding rectangle not found", procName, 1);
            }
        }

            /* Make a new ptaa, removing any old one */
        nb = numaaGetCount(naa);   /* number of borders */
        if ((ptaan = ptaaCreate(nb)) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("ptaan not made", procName, 1);
        }
        if (coordtype == CCB_LOCAL_COORDS) {
            if (ccb->local)   /* remove old one */
                ptaaDestroy(&ccb->local);
            ccb->local = ptaan;  /* save new local chain */
        } else {   /* coordtype == CCB_GLOBAL_COORDS */
            if (ccb->global)   /* remove old one */
                ptaaDestroy(&ccb->global);
            ccb->global = ptaan;  /* save new global chain */
        }

            /* Iterate through the borders for this c.c. */
        for (j = 0; j < nb; j++) {
            na = numaaGetNuma(naa, j, L_CLONE);
            n = numaGetCount(na);   /* number of steps in border */
            if ((ptan = ptaCreate(n + 1)) == NULL) {
                ccbDestroy(&ccb);
                numaDestroy(&na);
                return ERROR_INT("ptan not made", procName, 1);
            }
            ptaaAddPta(ptaan, ptan, L_INSERT);
            ptaGetIPt(ptas, j, &xstart, &ystart);
            x = xul + xstart;
            y = yul + ystart;
            ptaAddPt(ptan, x, y);
            for (k = 0; k < n; k++) {
                numaGetIValue(na, k, &stepdir);
                x += xpostab[stepdir];
                y += ypostab[stepdir];
                ptaAddPt(ptan, x, y);
            }
            numaDestroy(&na);
        }
        ccbDestroy(&ccb);
    }

    return 0;
}