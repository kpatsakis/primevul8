cmsBool WriteSetOfCurves(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsTagTypeSignature Type, cmsStage* mpe)
{
    cmsUInt32Number i, n;
    cmsTagTypeSignature CurrentType;
    cmsToneCurve** Curves;


    n      = cmsStageOutputChannels(mpe);
    Curves = _cmsStageGetPtrToCurveSet(mpe);

    for (i=0; i < n; i++) {

        // If this is a table-based curve, use curve type even on V4
        CurrentType = Type;

        if ((Curves[i] ->nSegments == 0)||
            ((Curves[i]->nSegments == 2) && (Curves[i] ->Segments[1].Type == 0)) )
            CurrentType = cmsSigCurveType;
        else
        if (Curves[i] ->Segments[0].Type < 0)
            CurrentType = cmsSigCurveType;

        if (!_cmsWriteTypeBase(io, CurrentType)) return FALSE;

        switch (CurrentType) {

            case cmsSigCurveType:
                if (!Type_Curve_Write(self, io, Curves[i], 1)) return FALSE;
                break;

            case cmsSigParametricCurveType:
                if (!Type_ParametricCurve_Write(self, io, Curves[i], 1)) return FALSE;
                break;

            default:
                {
                    char String[5];

                    _cmsTagSignature2String(String, (cmsTagSignature) Type);
                    cmsSignalError(self ->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown curve type '%s'", String);
                }
                return FALSE;
        }

        if (!_cmsWriteAlignment(io)) return FALSE;
    }


    return TRUE;
}