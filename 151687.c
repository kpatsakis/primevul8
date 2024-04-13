void *Type_ColorantOrderType_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsUInt8Number* ColorantOrder;
    cmsUInt32Number Count;

    *nItems = 0;
    if (!_cmsReadUInt32Number(io, &Count)) return NULL;
    if (Count > cmsMAXCHANNELS) return NULL;

    ColorantOrder = (cmsUInt8Number*) _cmsCalloc(self ->ContextID, cmsMAXCHANNELS, sizeof(cmsUInt8Number));
    if (ColorantOrder == NULL) return NULL;

    // We use FF as end marker
    memset(ColorantOrder, 0xFF, cmsMAXCHANNELS * sizeof(cmsUInt8Number));

    if (io ->Read(io, ColorantOrder, sizeof(cmsUInt8Number), Count) != Count) {

        _cmsFree(self ->ContextID, (void*) ColorantOrder);
        return NULL;
    }

    *nItems = 1;
    return (void*) ColorantOrder;

    cmsUNUSED_PARAMETER(SizeOfTag);
}