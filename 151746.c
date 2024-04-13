void Type_DateTime_Free(struct _cms_typehandler_struct* self, void* Ptr)
{
    _cmsFree(self ->ContextID, Ptr);
}