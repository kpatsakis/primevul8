cmsStage* ReadSetOfCurves(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number Offset, cmsUInt32Number nCurves)
{
    cmsToneCurve* Curves[cmsMAXCHANNELS];
    cmsUInt32Number i;
    cmsStage* Lin = NULL;

    if (nCurves > cmsMAXCHANNELS) return FALSE;

    if (!io -> Seek(io, Offset)) return FALSE;

    for (i=0; i < nCurves; i++)
        Curves[i] = NULL;

    for (i=0; i < nCurves; i++) {

        Curves[i] = ReadEmbeddedCurve(self, io);
        if (Curves[i] == NULL) goto Error;
        if (!_cmsReadAlignment(io)) goto Error;

    }

    Lin = cmsStageAllocToneCurves(self ->ContextID, nCurves, Curves);

Error:
    for (i=0; i < nCurves; i++)
        cmsFreeToneCurve(Curves[i]);

    return Lin;
}