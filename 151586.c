void* Type_Data_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    cmsICCData* BinData = (cmsICCData*) Ptr;

    return _cmsDupMem(self ->ContextID, Ptr, sizeof(cmsICCData) + BinData ->len - 1);

    cmsUNUSED_PARAMETER(n);
}