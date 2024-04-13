pixGetOuterBorder(CCBORD   *ccb,
                  PIX      *pixs,
                  BOX      *box)
{
l_int32    fpx, fpy, spx, spy, qpos;
l_int32    px, py, npx, npy;
l_int32    w, h, wpl;
l_uint32  *data;
PTA       *pta;
PIX       *pixb;  /* with 1 pixel border */

    PROCNAME("pixGetOuterBorder");

    if (!ccb)
        return ERROR_INT("ccb not defined", procName, 1);
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (!box)
        return ERROR_INT("box not defined", procName, 1);

        /* Add 1-pixel border all around, and find start pixel */
    if ((pixb = pixAddBorder(pixs, 1, 0)) == NULL)
        return ERROR_INT("pixs not made", procName, 1);
    if (!nextOnPixelInRaster(pixb, 1, 1, &px, &py)) {
        pixDestroy(&pixb);
        return ERROR_INT("no start pixel found", procName, 1);
    }
    qpos = 0;   /* relative to p */
    fpx = px;  /* save location of first pixel on border */
    fpy = py;

        /* Save box and start pixel in relative coords */
    boxaAddBox(ccb->boxa, box, L_COPY);
    ptaAddPt(ccb->start, px - 1, py - 1);

    pta = ptaCreate(0);
    ptaaAddPta(ccb->local, pta, L_INSERT);
    ptaAddPt(pta, px - 1, py - 1);   /* initial point */
    pixGetDimensions(pixb, &w, &h, NULL);
    data = pixGetData(pixb);
    wpl = pixGetWpl(pixb);

        /* Get the second point; if there is none, return */
    if (findNextBorderPixel(w, h, data, wpl, px, py, &qpos, &npx, &npy)) {
        pixDestroy(&pixb);
        return 0;
    }

    spx = npx;  /* save location of second pixel on border */
    spy = npy;
    ptaAddPt(pta, npx - 1, npy - 1);   /* second point */
    px = npx;
    py = npy;

    while (1) {
        findNextBorderPixel(w, h, data, wpl, px, py, &qpos, &npx, &npy);
        if (px == fpx && py == fpy && npx == spx && npy == spy)
            break;
        ptaAddPt(pta, npx - 1, npy - 1);
        px = npx;
        py = npy;
    }

    pixDestroy(&pixb);
    return 0;
}