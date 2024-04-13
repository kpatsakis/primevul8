ccbaCreate(PIX     *pixs,
           l_int32  n)
{
CCBORDA  *ccba;

    PROCNAME("ccbaCreate");

    if (n <= 0)
        n = INITIAL_PTR_ARRAYSIZE;

    ccba = (CCBORDA *)LEPT_CALLOC(1, sizeof(CCBORDA));
    if (pixs) {
        ccba->pix = pixClone(pixs);
        ccba->w = pixGetWidth(pixs);
        ccba->h = pixGetHeight(pixs);
    }
    ccba->n = 0;
    ccba->nalloc = n;
    if ((ccba->ccb = (CCBORD **)LEPT_CALLOC(n, sizeof(CCBORD *))) == NULL) {
        ccbaDestroy(&ccba);
        return (CCBORDA *)ERROR_PTR("ccba ptrs not made", procName, NULL);
    }
    return ccba;
}