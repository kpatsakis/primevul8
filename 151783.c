void *Type_Screening_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsScreening* sc = NULL;
    cmsUInt32Number i;

    sc = (cmsScreening*) _cmsMallocZero(self ->ContextID, sizeof(cmsScreening));
    if (sc == NULL) return NULL;

    *nItems = 0;

    if (!_cmsReadUInt32Number(io, &sc ->Flag)) goto Error;
    if (!_cmsReadUInt32Number(io, &sc ->nChannels)) goto Error;

    if (sc ->nChannels > cmsMAXCHANNELS - 1)
        sc ->nChannels = cmsMAXCHANNELS - 1;

    for (i=0; i < sc ->nChannels; i++) {

        if (!_cmsRead15Fixed16Number(io, &sc ->Channels[i].Frequency)) goto Error;
        if (!_cmsRead15Fixed16Number(io, &sc ->Channels[i].ScreenAngle)) goto Error;
        if (!_cmsReadUInt32Number(io, &sc ->Channels[i].SpotShape)) goto Error;
    }


    *nItems = 1;

    return (void*) sc;

Error:
    if (sc != NULL)
        _cmsFree(self ->ContextID, sc);

    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}