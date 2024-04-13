cmsBool Type_Data_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
   cmsICCData* BinData = (cmsICCData*) Ptr;

   if (!_cmsWriteUInt32Number(io, BinData ->flag)) return FALSE;

   return io ->Write(io, BinData ->len, BinData ->data);

   cmsUNUSED_PARAMETER(nItems);
   cmsUNUSED_PARAMETER(self);
}