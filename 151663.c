cmsBool  Type_MPEmatrix_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsUInt32Number i, nElems;
    cmsStage* mpe = (cmsStage*) Ptr;
    _cmsStageMatrixData* Matrix = (_cmsStageMatrixData*) mpe ->Data;

    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) mpe ->InputChannels)) return FALSE;
    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) mpe ->OutputChannels)) return FALSE;

    nElems = mpe ->InputChannels * mpe ->OutputChannels;

    for (i=0; i < nElems; i++) {
        if (!_cmsWriteFloat32Number(io, (cmsFloat32Number) Matrix->Double[i])) return FALSE;
    }


    for (i=0; i < mpe ->OutputChannels; i++) {

        if (Matrix ->Offset == NULL) {

               if (!_cmsWriteFloat32Number(io, 0)) return FALSE;
        }
        else {
               if (!_cmsWriteFloat32Number(io, (cmsFloat32Number) Matrix->Offset[i])) return FALSE;
        }
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}