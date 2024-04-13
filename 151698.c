void* Type_S15Fixed16_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    return _cmsDupMem(self ->ContextID, Ptr, n * sizeof(cmsFloat64Number));
}