ccbaWriteSVGString(const char  *filename,
                   CCBORDA     *ccba)
{
char    *svgstr;
char     smallbuf[256];
char     line0[] = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>";
char     line1[] = "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000303 Stylable//EN\" \"http://www.w3.org/TR/2000/03/WD-SVG-20000303/DTD/svg-20000303-stylable.dtd\">";
char     line2[] = "<svg>";
char     line3[] = "<polygon style=\"stroke-width:1;stroke:black;\" points=\"";
char     line4[] = "\" />";
char     line5[] = "</svg>";
char     space[] = " ";
l_int32  i, j, ncc, npt, x, y;
CCBORD  *ccb;
PTA     *pta;
SARRAY  *sa;

    PROCNAME("ccbaWriteSVGString");

    if (!filename)
        return (char *)ERROR_PTR("filename not defined", procName, NULL);
    if (!ccba)
        return (char *)ERROR_PTR("ccba not defined", procName, NULL);

    sa = sarrayCreate(0);
    sarrayAddString(sa, line0, L_COPY);
    sarrayAddString(sa, line1, L_COPY);
    sarrayAddString(sa, line2, L_COPY);
    ncc = ccbaGetCount(ccba);
    for (i = 0; i < ncc; i++) {
        if ((ccb = ccbaGetCcb(ccba, i)) == NULL) {
            sarrayDestroy(&sa);
            return (char *)ERROR_PTR("ccb not found", procName, NULL);
        }
        if ((pta = ccb->spglobal) == NULL) {
            sarrayDestroy(&sa);
            ccbDestroy(&ccb);
            return (char *)ERROR_PTR("spglobal not made", procName, NULL);
        }
        sarrayAddString(sa, line3, L_COPY);
        npt = ptaGetCount(pta);
        for (j = 0; j < npt; j++) {
            ptaGetIPt(pta, j, &x, &y);
            snprintf(smallbuf, sizeof(smallbuf), "%0d,%0d", x, y);
            sarrayAddString(sa, smallbuf, L_COPY);
        }
        sarrayAddString(sa, line4, L_COPY);
        ccbDestroy(&ccb);
    }
    sarrayAddString(sa, line5, L_COPY);
    sarrayAddString(sa, space, L_COPY);

    svgstr = sarrayToString(sa, 1);
/*    lept_stderr("%s", svgstr); */

    sarrayDestroy(&sa);
    return svgstr;
}