ccbaGenerateSinglePath(CCBORDA  *ccba)
{
l_int32   i, j, k, ncc, nb, ncut, npt, dir, len, state, lostholes;
l_int32   x, y, xl, yl, xf, yf;
BOX      *boxinner;
BOXA     *boxa;
CCBORD   *ccb;
PTA      *pta, *ptac, *ptah;
PTA      *ptahc;  /* cyclic permutation of hole border, with end pts at cut */
PTA      *ptas;  /* output result: new single path for c.c. */
PTA      *ptaf;  /* points on the hole borders that intersect with cuts */
PTA      *ptal;  /* points on outer border that intersect with cuts */
PTA      *ptap, *ptarp;   /* path and reverse path between borders */
PTAA     *ptaa;
PTAA     *ptaap;  /* ptaa for all paths between borders */

    PROCNAME("ccbaGenerateSinglePath");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    ncc = ccbaGetCount(ccba);   /* number of c.c. */
    lostholes = 0;
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);
        if ((ptaa = ccb->local) == NULL) {
            L_WARNING("local pixel loc array not found\n", procName);
            continue;
        }
        nb = ptaaGetCount(ptaa);   /* number of borders in the c.c.  */

            /* Prepare the output pta */
        if (ccb->splocal)
            ptaDestroy(&ccb->splocal);
        ptas = ptaCreate(0);
        ccb->splocal = ptas;

            /* If no holes, just concat the outer border */
        pta = ptaaGetPta(ptaa, 0, L_CLONE);
        if (nb == 1 || nb > NMAX_HOLES + 1) {
            ptaJoin(ptas, pta, 0, -1);
            ptaDestroy(&pta);  /* remove clone */
            ccbDestroy(&ccb);  /* remove clone */
            continue;
        }

            /* Find the (nb - 1) cut paths that connect holes
             * with outer border */
        boxa = ccb->boxa;
        ptaap = ptaaCreate(nb - 1);
        ptaf = ptaCreate(nb - 1);
        ptal = ptaCreate(nb - 1);
        for (j = 1; j < nb; j++) {
            boxinner = boxaGetBox(boxa, j, L_CLONE);

                /* Find a short path and store it */
            ptac = getCutPathForHole(ccb->pix, pta, boxinner, &dir, &len);
            if (len == 0) {  /* bad: we lose the hole! */
                lostholes++;
/*                boxPrintStreamInfo(stderr, boxa->box[0]); */
            }
            ptaaAddPta(ptaap, ptac, L_INSERT);
/*            lept_stderr("dir = %d, length = %d\n", dir, len); */
/*            ptaWriteStream(stderr, ptac, 1); */

                /* Store the first and last points in the cut path,
                 * which must be on a hole border and the outer
                 * border, respectively */
            ncut = ptaGetCount(ptac);
            if (ncut == 0) {   /* missed hole; neg coords won't match */
                ptaAddPt(ptaf, -1, -1);
                ptaAddPt(ptal, -1, -1);
            } else {
                ptaGetIPt(ptac, 0, &x, &y);
                ptaAddPt(ptaf, x, y);
                ptaGetIPt(ptac, ncut - 1, &x, &y);
                ptaAddPt(ptal, x, y);
            }
            boxDestroy(&boxinner);
        }

            /* Make a single path for the c.c. using these connections */
        npt = ptaGetCount(pta);  /* outer border pts */
        for (k = 0; k < npt; k++) {
            ptaGetIPt(pta, k, &x, &y);
            if (k == 0) {   /* if there is a cut at the first point,
                             * we can wait until the end to take it */
                ptaAddPt(ptas, x, y);
                continue;
            }
            state = L_NOT_FOUND;
            for (j = 0; j < nb - 1; j++) {  /* iterate over cut end pts */
                ptaGetIPt(ptal, j, &xl, &yl);  /* cut point on outer border */
                if (x == xl && y == yl) {  /* take this cut to the hole */
                    state = L_FOUND;
                    ptap = ptaaGetPta(ptaap, j, L_CLONE);
                    ptarp = ptaReverse(ptap, 1);
                        /* Cut point on hole border: */
                    ptaGetIPt(ptaf, j, &xf, &yf);
                        /* Hole border: */
                    ptah = ptaaGetPta(ptaa, j + 1, L_CLONE);
                    ptahc = ptaCyclicPerm(ptah, xf, yf);
/*                    ptaWriteStream(stderr, ptahc, 1); */
                    ptaJoin(ptas, ptarp, 0, -1);
                    ptaJoin(ptas, ptahc, 0, -1);
                    ptaJoin(ptas, ptap, 0, -1);
                    ptaDestroy(&ptap);
                    ptaDestroy(&ptarp);
                    ptaDestroy(&ptah);
                    ptaDestroy(&ptahc);
                    break;
                }
            }
            if (state == L_NOT_FOUND)
                ptaAddPt(ptas, x, y);
        }

/*        ptaWriteStream(stderr, ptas, 1); */
        ptaaDestroy(&ptaap);
        ptaDestroy(&ptaf);
        ptaDestroy(&ptal);
        ptaDestroy(&pta);  /* remove clone */
        ccbDestroy(&ccb);  /* remove clone */
    }

    if (lostholes > 0)
        L_WARNING("***** %d lost holes *****\n", procName, lostholes);

    return 0;
}