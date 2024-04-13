cmsBool Type_ColorantOrderType_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsUInt8Number*  ColorantOrder = (cmsUInt8Number*) Ptr;
    cmsUInt32Number i, sz, Count;

    // Get the length
    for (Count=i=0; i < cmsMAXCHANNELS; i++) {
        if (ColorantOrder[i] != 0xFF) Count++;
    }

    if (!_cmsWriteUInt32Number(io, Count)) return FALSE;

    sz = Count * sizeof(cmsUInt8Number);
    if (!io -> Write(io, sz, ColorantOrder)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}