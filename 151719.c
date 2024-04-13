cmsBool Type_Screening_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsScreening* sc = (cmsScreening* ) Ptr;
    cmsUInt32Number i;

    if (!_cmsWriteUInt32Number(io, sc ->Flag)) return FALSE;
    if (!_cmsWriteUInt32Number(io, sc ->nChannels)) return FALSE;

    for (i=0; i < sc ->nChannels; i++) {

        if (!_cmsWrite15Fixed16Number(io, sc ->Channels[i].Frequency)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, sc ->Channels[i].ScreenAngle)) return FALSE;
        if (!_cmsWriteUInt32Number(io, sc ->Channels[i].SpotShape)) return FALSE;
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}