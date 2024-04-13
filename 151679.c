cmsBool WriteSeqID(struct _cms_typehandler_struct* self,
                                             cmsIOHANDLER* io,
                                             void* Cargo,
                                             cmsUInt32Number n,
                                             cmsUInt32Number SizeOfTag)
{
    cmsSEQ* Seq = (cmsSEQ*) Cargo;

    if (!io ->Write(io, 16, Seq ->seq[n].ProfileID.ID8)) return FALSE;

    // Store here the MLU
    if (!SaveDescription(self, io, Seq ->seq[n].Description)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(SizeOfTag);
}