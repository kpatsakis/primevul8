void* Type_Screening_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
   return _cmsDupMem(self ->ContextID, Ptr, sizeof(cmsScreening));

   cmsUNUSED_PARAMETER(n);
}