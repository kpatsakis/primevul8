cmsBool  Type_Signature_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsSignature* SigPtr = (cmsSignature*) Ptr;

    return _cmsWriteUInt32Number(io, *SigPtr);

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}