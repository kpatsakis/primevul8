void *Type_Signature_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsSignature* SigPtr = (cmsSignature*) _cmsMalloc(self ->ContextID, sizeof(cmsSignature));
    if (SigPtr == NULL) return NULL;

     if (!_cmsReadUInt32Number(io, SigPtr)) return NULL;
     *nItems = 1;

     return SigPtr;

     cmsUNUSED_PARAMETER(SizeOfTag);
}