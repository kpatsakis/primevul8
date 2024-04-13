pixGetOuterBordersPtaa(PIX  *pixs)
{
l_int32  i, n;
BOX     *box;
BOXA    *boxa;
PIX     *pix;
PIXA    *pixa;
PTA     *pta;
PTAA    *ptaa;

    PROCNAME("pixGetOuterBordersPtaa");

    if (!pixs)
        return (PTAA *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 1)
        return (PTAA *)ERROR_PTR("pixs not binary", procName, NULL);

    boxa = pixConnComp(pixs, &pixa, 8);
    n = boxaGetCount(boxa);
    if (n == 0) {
        boxaDestroy(&boxa);
        pixaDestroy(&pixa);
        return (PTAA *)ERROR_PTR("pixs empty", procName, NULL);
    }

    ptaa = ptaaCreate(n);
    for (i = 0; i < n; i++) {
        box = boxaGetBox(boxa, i, L_CLONE);
        pix = pixaGetPix(pixa, i, L_CLONE);
        pta = pixGetOuterBorderPta(pix, box);
        if (pta)
            ptaaAddPta(ptaa, pta, L_INSERT);
        boxDestroy(&box);
        pixDestroy(&pix);
    }

    pixaDestroy(&pixa);
    boxaDestroy(&boxa);
    return ptaa;
}