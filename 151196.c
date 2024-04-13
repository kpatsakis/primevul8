pixGetHoleBorder(CCBORD   *ccb,
                 PIX      *pixs,
                 BOX      *box,
                 l_int32   xs,
                 l_int32   ys)
{
l_int32    fpx, fpy, spx, spy, qpos;
l_int32    px, py, npx, npy;
l_int32    w, h, wpl;
l_uint32  *data;
PTA       *pta;

    PROCNAME("pixGetHoleBorder");

    if (!ccb)
        return ERROR_INT("ccb not defined", procName, 1);
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (!box)
        return ERROR_INT("box not defined", procName, 1);

        /* Add border and find start pixel */
    qpos = 0;   /* orientation of Q relative to P */
    fpx = xs;  /* save location of first pixel on border */
    fpy = ys;

        /* Save box and start pixel */
    boxaAddBox(ccb->boxa, box, L_COPY);
    ptaAddPt(ccb->start, xs, ys);

    pta = ptaCreate(0);
    ptaaAddPta(ccb->local, pta, L_INSERT);
    ptaAddPt(pta, xs, ys);   /* initial pixel */

    w = pixGetWidth(pixs);
    h = pixGetHeight(pixs);
    data = pixGetData(pixs);
    wpl = pixGetWpl(pixs);

        /* Get the second point; there should always be at least 4 pts
         * in a minimal hole border!  */
    if (findNextBorderPixel(w, h, data, wpl, xs, ys, &qpos, &npx, &npy))
        return ERROR_INT("isolated hole border point!", procName, 1);

    spx = npx;  /* save location of second pixel on border */
    spy = npy;
    ptaAddPt(pta, npx, npy);   /* second pixel */
    px = npx;
    py = npy;

    while (1) {
        findNextBorderPixel(w, h, data, wpl, px, py, &qpos, &npx, &npy);
        if (px == fpx && py == fpy && npx == spx && npy == spy)
            break;
        ptaAddPt(pta, npx, npy);
        px = npx;
        py = npy;
    }

    return 0;
}