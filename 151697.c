cmsBool  Type_ProfileSequenceDesc_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsSEQ* Seq = (cmsSEQ*) Ptr;
    cmsUInt32Number i;

    if (!_cmsWriteUInt32Number(io, Seq->n)) return FALSE;

    for (i=0; i < Seq ->n; i++) {

        cmsPSEQDESC* sec = &Seq -> seq[i];

        if (!_cmsWriteUInt32Number(io, sec ->deviceMfg)) return FALSE;
        if (!_cmsWriteUInt32Number(io, sec ->deviceModel)) return FALSE;
        if (!_cmsWriteUInt64Number(io, &sec ->attributes)) return FALSE;
        if (!_cmsWriteUInt32Number(io, sec ->technology)) return FALSE;

        if (!SaveDescription(self, io, sec ->Manufacturer)) return FALSE;
        if (!SaveDescription(self, io, sec ->Model)) return FALSE;
    }

     return TRUE;

     cmsUNUSED_PARAMETER(nItems);
}