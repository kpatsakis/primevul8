void* Type_ColorantOrderType_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    return _cmsDupMem(self ->ContextID, Ptr, cmsMAXCHANNELS * sizeof(cmsUInt8Number));

    cmsUNUSED_PARAMETER(n);
}