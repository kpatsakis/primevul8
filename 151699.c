void* Type_Text_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    return (void*) cmsMLUdup((cmsMLU*) Ptr);

    cmsUNUSED_PARAMETER(n);
    cmsUNUSED_PARAMETER(self);
}