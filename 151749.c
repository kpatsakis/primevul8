cmsBool  Type_MPEcurve_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsUInt32Number BaseOffset;
    cmsStage* mpe = (cmsStage*) Ptr;
    _cmsStageToneCurvesData* Curves = (_cmsStageToneCurvesData*) mpe ->Data;

    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    // Write the header. Since those are curves, input and output channels are same
    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) mpe ->InputChannels)) return FALSE;
    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) mpe ->InputChannels)) return FALSE;

    if (!WritePositionTable(self, io, 0,
                                mpe ->InputChannels, BaseOffset, Curves, WriteMPECurve)) return FALSE;


    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
}