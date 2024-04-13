pixGetCCBorders(PIX      *pixs,
                BOX      *box)
{
l_int32   allzero, i, x, xh, w, nh;
l_int32   xs, ys;   /* starting hole border pixel, relative in pixs */
l_uint32  val;
BOX      *boxt, *boxe;
BOXA     *boxa;
CCBORD   *ccb;
PIX      *pixh;  /* for hole components */
PIX      *pixt;
PIXA     *pixa;

    PROCNAME("pixGetCCBorders");

    if (!pixs)
        return (CCBORD *)ERROR_PTR("pixs not defined", procName, NULL);
    if (!box)
        return (CCBORD *)ERROR_PTR("box not defined", procName, NULL);
    if (pixGetDepth(pixs) != 1)
        return (CCBORD *)ERROR_PTR("pixs not binary", procName, NULL);

    pixZero(pixs, &allzero);
    if (allzero)
        return (CCBORD *)ERROR_PTR("pixs all 0", procName, NULL);

    if ((ccb = ccbCreate(pixs)) == NULL)
        return (CCBORD *)ERROR_PTR("ccb not made", procName, NULL);

        /* Get the exterior border */
    pixGetOuterBorder(ccb, pixs, box);

        /* Find the holes, if any */
    if ((pixh = pixHolesByFilling(pixs, 4)) == NULL) {
        ccbDestroy(&ccb);
        return (CCBORD *)ERROR_PTR("pixh not made", procName, NULL);
    }
    pixZero(pixh, &allzero);
    if (allzero) {  /* no holes */
        pixDestroy(&pixh);
        return ccb;
    }

        /* Get c.c. and locations of the holes */
    if ((boxa = pixConnComp(pixh, &pixa, 4)) == NULL) {
        ccbDestroy(&ccb);
        pixDestroy(&pixh);
        return (CCBORD *)ERROR_PTR("boxa not made", procName, NULL);
    }
    nh = boxaGetCount(boxa);
/*    lept_stderr("%d holes\n", nh); */

        /* For each hole, find an interior pixel within the hole,
         * then march to the right and stop at the first border
         * pixel.  Save the bounding box of the border, which
         * is 1 pixel bigger on each side than the bounding box
         * of the hole itself.  Note that we use a pix of the
         * c.c. of the hole itself to be sure that we start
         * with a pixel in the hole of the proper component.
         * If we did everything from the parent component, it is
         * possible to start in a different hole that is within
         * the b.b. of a larger hole.  */
    w = pixGetWidth(pixs);
    for (i = 0; i < nh; i++) {
        boxt = boxaGetBox(boxa, i, L_CLONE);
        pixt = pixaGetPix(pixa, i, L_CLONE);
        ys = boxt->y;   /* there must be a hole pixel on this raster line */
        for (x = 0; x < boxt->w; x++) {  /* look for (fg) hole pixel */
            pixGetPixel(pixt, x, 0, &val);
            if (val == 1) {
                xh = x;
                break;
            }
        }
        if (x == boxt->w) {
            L_WARNING("no hole pixel found!\n", procName);
            continue;
        }
        for (x = xh + boxt->x; x < w; x++) {  /* look for (fg) border pixel */
            pixGetPixel(pixs, x, ys, &val);
            if (val == 1) {
                xs = x;
                break;
            }
        }
        boxe = boxCreate(boxt->x - 1, boxt->y - 1, boxt->w + 2, boxt->h + 2);
#if  DEBUG_PRINT
        boxPrintStreamInfo(stderr, box);
        boxPrintStreamInfo(stderr, boxe);
        lept_stderr("xs = %d, ys = %d\n", xs, ys);
#endif   /* DEBUG_PRINT */
        pixGetHoleBorder(ccb, pixs, boxe, xs, ys);
        boxDestroy(&boxt);
        boxDestroy(&boxe);
        pixDestroy(&pixt);
    }

    boxaDestroy(&boxa);
    pixaDestroy(&pixa);
    pixDestroy(&pixh);
    return ccb;
}