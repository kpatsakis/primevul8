void Type_Text_Description_Free(struct _cms_typehandler_struct* self, void* Ptr)
{
    cmsMLU* mlu = (cmsMLU*) Ptr;

    cmsMLUfree(mlu);
    return;

    cmsUNUSED_PARAMETER(self);
}