ccbaGenerateSPGlobalLocs(CCBORDA  *ccba,
                         l_int32   ptsflag)
{
l_int32  ncc, npt, i, j, xul, yul, x, y, delx, dely;
l_int32  xp, yp, delxp, delyp;   /* prev point and increments */
CCBORD  *ccb;
PTA     *ptal, *ptag;

    PROCNAME("ccbaGenerateSPGlobalLocs");

    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

        /* Make sure we have a local single path representation */
    if ((ccb = ccbaGetCcb(ccba, 0)) == NULL)
        return ERROR_INT("no ccb", procName, 1);
    if (!ccb->splocal)
        ccbaGenerateSinglePath(ccba);
    ccbDestroy(&ccb);  /* clone ref */

    ncc = ccbaGetCount(ccba);  /* number of c.c. */
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);

            /* Get the UL corner in global coords, (xul, yul), of the c.c. */
        if (boxaGetBoxGeometry(ccb->boxa, 0, &xul, &yul, NULL, NULL)) {
            ccbDestroy(&ccb);
            return ERROR_INT("bounding rectangle not found", procName, 1);
        }

            /* Make a new spglobal pta, removing any old one */
        ptal = ccb->splocal;
        npt = ptaGetCount(ptal);   /* number of points */
        if (ccb->spglobal)   /* remove old one */
            ptaDestroy(&ccb->spglobal);
        if ((ptag = ptaCreate(npt)) == NULL) {
            ccbDestroy(&ccb);
            return ERROR_INT("ptag not made", procName, 1);
        }
        ccb->spglobal = ptag;  /* save new one */

            /* Convert local to global */
        if (ptsflag == CCB_SAVE_ALL_PTS) {
            for (j = 0; j < npt; j++) {
                ptaGetIPt(ptal, j, &x, &y);
                ptaAddPt(ptag, x  + xul, y + yul);
            }
        } else {   /* ptsflag = CCB_SAVE_TURNING_PTS */
            ptaGetIPt(ptal, 0, &xp, &yp);   /* get the 1st pt */
            ptaAddPt(ptag, xp  + xul, yp + yul);   /* save the 1st pt */
            if (npt == 2) {  /* get and save the 2nd pt  */
                ptaGetIPt(ptal, 1, &x, &y);
                ptaAddPt(ptag, x  + xul, y + yul);
            } else if (npt > 2)  {
                ptaGetIPt(ptal, 1, &x, &y);
                delxp = x - xp;
                delyp = y - yp;
                xp = x;
                yp = y;
                for (j = 2; j < npt; j++) {
                    ptaGetIPt(ptal, j, &x, &y);
                    delx = x - xp;
                    dely = y - yp;
                    if (delx != delxp || dely != delyp)
                        ptaAddPt(ptag, xp  + xul, yp + yul);
                    xp = x;
                    yp = y;
                    delxp = delx;
                    delyp = dely;
                }
                ptaAddPt(ptag, xp  + xul, yp + yul);
            }
        }

        ccbDestroy(&ccb);  /* clone ref */
    }

    return 0;
}