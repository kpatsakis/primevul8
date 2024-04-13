void *Type_MPEcurve_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsStage* mpe = NULL;
    cmsUInt16Number InputChans, OutputChans;
    cmsUInt32Number i, BaseOffset;
    cmsToneCurve** GammaTables;

    *nItems = 0;

    // Get actual position as a basis for element offsets
    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    if (!_cmsReadUInt16Number(io, &InputChans)) return NULL;
    if (!_cmsReadUInt16Number(io, &OutputChans)) return NULL;

    if (InputChans != OutputChans) return NULL;

    GammaTables = (cmsToneCurve**) _cmsCalloc(self ->ContextID, InputChans, sizeof(cmsToneCurve*));
    if (GammaTables == NULL) return NULL;

    if (ReadPositionTable(self, io, InputChans, BaseOffset, GammaTables, ReadMPECurve)) {

        mpe = cmsStageAllocToneCurves(self ->ContextID, InputChans, GammaTables);
    }
    else {
        mpe = NULL;
    }

    for (i=0; i < InputChans; i++) {
        if (GammaTables[i]) cmsFreeToneCurve(GammaTables[i]);
    }

    _cmsFree(self ->ContextID, GammaTables);
    *nItems = (mpe != NULL) ? 1 : 0;
    return mpe;

    cmsUNUSED_PARAMETER(SizeOfTag);
}