getCutPathForHole(PIX      *pix,
                  PTA      *pta,
                  BOX      *boxinner,
                  l_int32  *pdir,
                  l_int32  *plen)
{
l_int32   w, h, nc, x, y, xl, yl, xmid, ymid;
l_uint32  val;
PTA      *ptac;

    PROCNAME("getCutPathForHole");

    if (!pix)
        return (PTA *)ERROR_PTR("pix not defined", procName, NULL);
    if (!pta)
        return (PTA *)ERROR_PTR("pta not defined", procName, NULL);
    if (!boxinner)
        return (PTA *)ERROR_PTR("boxinner not defined", procName, NULL);

    pixGetDimensions(pix, &w, &h, NULL);
    ptac = ptaCreate(4);
    xmid = boxinner->x + boxinner->w / 2;
    ymid = boxinner->y + boxinner->h / 2;

        /* try top first */
    for (y = ymid; y >= 0; y--) {
        pixGetPixel(pix, xmid, y, &val);
        if (val == 1) {
            ptaAddPt(ptac, xmid, y);
            break;
        }
    }
    for (y = y - 1; y >= 0; y--) {
        pixGetPixel(pix, xmid, y, &val);
        if (val == 1)
            ptaAddPt(ptac, xmid, y);
        else
            break;
    }
    nc = ptaGetCount(ptac);
    ptaGetIPt(ptac, nc - 1, &xl, &yl);
    if (ptaContainsPt(pta, xl, yl)) {
        *pdir = 1;
        *plen = nc;
        return ptac;
    }

        /* Next try bottom */
    ptaEmpty(ptac);
    for (y = ymid; y < h; y++) {
        pixGetPixel(pix, xmid, y, &val);
        if (val == 1) {
            ptaAddPt(ptac, xmid, y);
            break;
        }
    }
    for (y = y + 1; y < h; y++) {
        pixGetPixel(pix, xmid, y, &val);
        if (val == 1)
            ptaAddPt(ptac, xmid, y);
        else
            break;
    }
    nc = ptaGetCount(ptac);
    ptaGetIPt(ptac, nc - 1, &xl, &yl);
    if (ptaContainsPt(pta, xl, yl)) {
        *pdir = 3;
        *plen = nc;
        return ptac;
    }

        /* Next try left */
    ptaEmpty(ptac);
    for (x = xmid; x >= 0; x--) {
        pixGetPixel(pix, x, ymid, &val);
        if (val == 1) {
            ptaAddPt(ptac, x, ymid);
            break;
        }
    }
    for (x = x - 1; x >= 0; x--) {
        pixGetPixel(pix, x, ymid, &val);
        if (val == 1)
            ptaAddPt(ptac, x, ymid);
        else
            break;
    }
    nc = ptaGetCount(ptac);
    ptaGetIPt(ptac, nc - 1, &xl, &yl);
    if (ptaContainsPt(pta, xl, yl)) {
        *pdir = 0;
        *plen = nc;
        return ptac;
    }

        /* Finally try right */
    ptaEmpty(ptac);
    for (x = xmid; x < w; x++) {
        pixGetPixel(pix, x, ymid, &val);
        if (val == 1) {
            ptaAddPt(ptac, x, ymid);
            break;
        }
    }
    for (x = x + 1; x < w; x++) {
        pixGetPixel(pix, x, ymid, &val);
        if (val == 1)
            ptaAddPt(ptac, x, ymid);
        else
            break;
    }
    nc = ptaGetCount(ptac);
    ptaGetIPt(ptac, nc - 1, &xl, &yl);
    if (ptaContainsPt(pta, xl, yl)) {
        *pdir = 2;
        *plen = nc;
        return ptac;
    }

        /* If we get here, we've failed! */
    ptaEmpty(ptac);
    L_WARNING("no path found\n", procName);
    *plen = 0;
    return ptac;
}