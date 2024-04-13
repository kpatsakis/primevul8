void Type_Dictionary_Free(struct _cms_typehandler_struct* self, void* Ptr)
{
    cmsDictFree((cmsHANDLE) Ptr);
    cmsUNUSED_PARAMETER(self);
}