cmsBool Type_S15Fixed16_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsFloat64Number* Value = (cmsFloat64Number*) Ptr;
    cmsUInt32Number i;

    for (i=0; i < nItems; i++) {

        if (!_cmsWrite15Fixed16Number(io, Value[i])) return FALSE;
    }

    return TRUE;

    cmsUNUSED_PARAMETER(self);
}