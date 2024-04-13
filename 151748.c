void Type_CrdInfo_Free(struct _cms_typehandler_struct* self, void *Ptr)
{
    cmsMLUfree((cmsMLU*) Ptr);
    return;

    cmsUNUSED_PARAMETER(self);
}