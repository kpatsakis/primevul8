void* Type_ProfileSequenceId_Dup(struct _cms_typehandler_struct* self, const void* Ptr, cmsUInt32Number n)
{
    return (void*) cmsDupProfileSequenceDescription((cmsSEQ*) Ptr);

    cmsUNUSED_PARAMETER(n);
    cmsUNUSED_PARAMETER(self);
}