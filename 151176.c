locateOutsideSeedPixel(l_int32   fpx,
                       l_int32   fpy,
                       l_int32   spx,
                       l_int32   spy,
                       l_int32  *pxs,
                       l_int32  *pys)
{
l_int32  dx, dy;

    dx = spx - fpx;
    dy = spy - fpy;

    if (dx * dy == 1) {
        *pxs = fpx + dx;
        *pys = fpy;
    } else if (dx * dy == -1) {
        *pxs = fpx;
        *pys = fpy + dy;
    } else if (dx == 0) {
        *pxs = fpx + dy;
        *pys = fpy + dy;
    } else  /* dy == 0 */ {
        *pxs = fpx + dx;
        *pys = fpy - dx;
    }

    return;
}