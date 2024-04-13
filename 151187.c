ccbaGetCcb(CCBORDA  *ccba,
           l_int32   index)
{
CCBORD  *ccb;

    PROCNAME("ccbaGetCcb");

    if (!ccba)
        return (CCBORD *)ERROR_PTR("ccba not defined", procName, NULL);
    if (index < 0 || index >= ccba->n)
        return (CCBORD *)ERROR_PTR("index out of bounds", procName, NULL);

    ccb = ccba->ccb[index];
    ccb->refcount++;
    return ccb;
}