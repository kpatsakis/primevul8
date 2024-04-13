pixGetOuterBorderPta(PIX  *pixs,
                     BOX  *box)
{
l_int32  allzero, x, y;
BOX     *boxt;
CCBORD  *ccb;
PTA     *ptaloc, *ptad;

    PROCNAME("pixGetOuterBorderPta");

    if (!pixs)
        return (PTA *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 1)
        return (PTA *)ERROR_PTR("pixs not binary", procName, NULL);

    pixZero(pixs, &allzero);
    if (allzero)
        return (PTA *)ERROR_PTR("pixs all 0", procName, NULL);

    if ((ccb = ccbCreate(pixs)) == NULL)
        return (PTA *)ERROR_PTR("ccb not made", procName, NULL);
    if (!box)
        boxt = boxCreate(0, 0, pixGetWidth(pixs), pixGetHeight(pixs));
    else
        boxt = boxClone(box);

        /* Get the exterior border in local coords */
    pixGetOuterBorder(ccb, pixs, boxt);
    if ((ptaloc = ptaaGetPta(ccb->local, 0, L_CLONE)) == NULL) {
        ccbDestroy(&ccb);
        boxDestroy(&boxt);
        return (PTA *)ERROR_PTR("ptaloc not made", procName, NULL);
    }

        /* Transform to global coordinates, if they are given */
    if (box) {
        boxGetGeometry(box, &x, &y, NULL, NULL);
        ptad = ptaTransform(ptaloc, x, y, 1.0, 1.0);
    } else {
        ptad = ptaClone(ptaloc);
    }

    ptaDestroy(&ptaloc);
    boxDestroy(&boxt);
    ccbDestroy(&ccb);
    return ptad;
}