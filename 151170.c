ccbCreate(PIX  *pixs)
{
BOXA    *boxa;
CCBORD  *ccb;
PTA     *start;
PTAA    *local;

    PROCNAME("ccbCreate");

    if (pixs && pixGetDepth(pixs) != 1)  /* pixs can be null */
        return (CCBORD *)ERROR_PTR("pixs defined and not 1bpp", procName, NULL);

    ccb = (CCBORD *)LEPT_CALLOC(1, sizeof(CCBORD));
    ccb->refcount++;
    if (pixs)
        ccb->pix = pixClone(pixs);
    boxa = boxaCreate(1);
    ccb->boxa = boxa;
    start = ptaCreate(1);
    ccb->start = start;
    local = ptaaCreate(1);
    ccb->local = local;
    return ccb;
}