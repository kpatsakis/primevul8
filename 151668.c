void *Type_MPEmatrix_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsStage* mpe;
    cmsUInt16Number   InputChans, OutputChans;
    cmsUInt32Number   nElems, i;
    cmsFloat64Number* Matrix;
    cmsFloat64Number* Offsets;

    if (!_cmsReadUInt16Number(io, &InputChans)) return NULL;
    if (!_cmsReadUInt16Number(io, &OutputChans)) return NULL;


    // Input and output chans may be ANY (up to 0xffff), 
    // but we choose to limit to 16 channels for now
    if (InputChans >= cmsMAXCHANNELS) return NULL;
    if (OutputChans >= cmsMAXCHANNELS) return NULL;

    nElems = InputChans * OutputChans;

    Matrix = (cmsFloat64Number*) _cmsCalloc(self ->ContextID, nElems, sizeof(cmsFloat64Number));
    if (Matrix == NULL) return NULL;

    Offsets = (cmsFloat64Number*) _cmsCalloc(self ->ContextID, OutputChans, sizeof(cmsFloat64Number));
    if (Offsets == NULL) {

        _cmsFree(self ->ContextID, Matrix);
        return NULL;
    }

    for (i=0; i < nElems; i++) {

        cmsFloat32Number v;

        if (!_cmsReadFloat32Number(io, &v)) {
            _cmsFree(self ->ContextID, Matrix);
            _cmsFree(self ->ContextID, Offsets);
            return NULL;
        }
        Matrix[i] = v;
    }


    for (i=0; i < OutputChans; i++) {

        cmsFloat32Number v;

        if (!_cmsReadFloat32Number(io, &v)) {
            _cmsFree(self ->ContextID, Matrix);
            _cmsFree(self ->ContextID, Offsets);
            return NULL;
        }
        Offsets[i] = v;
    }


    mpe = cmsStageAllocMatrix(self ->ContextID, OutputChans, InputChans, Matrix, Offsets);
    _cmsFree(self ->ContextID, Matrix);
    _cmsFree(self ->ContextID, Offsets);

    *nItems = 1;

    return mpe;

    cmsUNUSED_PARAMETER(SizeOfTag);
}