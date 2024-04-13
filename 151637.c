cmsBool WriteSegmentedCurve(cmsIOHANDLER* io, cmsToneCurve* g)
{
    cmsUInt32Number i, j;
    cmsCurveSegment* Segments = g ->Segments;
    cmsUInt32Number nSegments = g ->nSegments;

    if (!_cmsWriteUInt32Number(io, cmsSigSegmentedCurve)) goto Error;
    if (!_cmsWriteUInt32Number(io, 0)) goto Error;
    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) nSegments)) goto Error;
    if (!_cmsWriteUInt16Number(io, 0)) goto Error;

    // Write the break-points
    for (i=0; i < nSegments - 1; i++) {
        if (!_cmsWriteFloat32Number(io, Segments[i].x1)) goto Error;
    }

    // Write the segments
    for (i=0; i < g ->nSegments; i++) {

        cmsCurveSegment* ActualSeg = Segments + i;

        if (ActualSeg -> Type == 0) {

            // This is a sampled curve
            if (!_cmsWriteUInt32Number(io, (cmsUInt32Number) cmsSigSampledCurveSeg)) goto Error;
            if (!_cmsWriteUInt32Number(io, 0)) goto Error;
            if (!_cmsWriteUInt32Number(io, ActualSeg -> nGridPoints)) goto Error;

            for (j=0; j < g ->Segments[i].nGridPoints; j++) {
                if (!_cmsWriteFloat32Number(io, ActualSeg -> SampledPoints[j])) goto Error;
            }

        }
        else {
            int Type;
            cmsUInt32Number ParamsByType[] = { 4, 5, 5 };

            // This is a formula-based
            if (!_cmsWriteUInt32Number(io, (cmsUInt32Number) cmsSigFormulaCurveSeg)) goto Error;
            if (!_cmsWriteUInt32Number(io, 0)) goto Error;

            // We only allow 1, 2 and 3 as types
            Type = ActualSeg ->Type - 6;
            if (Type > 2 || Type < 0) goto Error;

            if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) Type)) goto Error;
            if (!_cmsWriteUInt16Number(io, 0)) goto Error;

            for (j=0; j < ParamsByType[Type]; j++) {
                if (!_cmsWriteFloat32Number(io, (cmsFloat32Number) ActualSeg ->Params[j])) goto Error;
            }
        }

        // It seems there is no need to align. Code is here, and for safety commented out
        // if (!_cmsWriteAlignment(io)) goto Error;
    }

    return TRUE;

Error:
    return FALSE;
}