cmsBool _cmsOptimizePipeline(cmsContext ContextID,
                             cmsPipeline**    PtrLut,
                             int              Intent,
                             cmsUInt32Number* InputFormat,
                             cmsUInt32Number* OutputFormat,
                             cmsUInt32Number* dwFlags)
{
    _cmsOptimizationPluginChunkType* ctx = ( _cmsOptimizationPluginChunkType*) _cmsContextGetClientChunk(ContextID, OptimizationPlugin);
    _cmsOptimizationCollection* Opts;
    cmsBool AnySuccess = FALSE;

    // A CLUT is being asked, so force this specific optimization
    if (*dwFlags & cmsFLAGS_FORCE_CLUT) {

        PreOptimize(*PtrLut);
        return OptimizeByResampling(PtrLut, Intent, InputFormat, OutputFormat, dwFlags);
    }

    // Anything to optimize?
    if ((*PtrLut) ->Elements == NULL) {
        _cmsPipelineSetOptimizationParameters(*PtrLut, FastIdentity16, (void*) *PtrLut, NULL, NULL);
        return TRUE;
    }

    // Try to get rid of identities and trivial conversions.
    AnySuccess = PreOptimize(*PtrLut);

    // After removal do we end with an identity?
    if ((*PtrLut) ->Elements == NULL) {
        _cmsPipelineSetOptimizationParameters(*PtrLut, FastIdentity16, (void*) *PtrLut, NULL, NULL);
        return TRUE;
    }

    // Do not optimize, keep all precision
    if (*dwFlags & cmsFLAGS_NOOPTIMIZE)
        return FALSE;

    // Try plug-in optimizations 
    for (Opts = ctx->OptimizationCollection;
         Opts != NULL;
         Opts = Opts ->Next) {

            // If one schema succeeded, we are done
            if (Opts ->OptimizePtr(PtrLut, Intent, InputFormat, OutputFormat, dwFlags)) {

                return TRUE;    // Optimized!
            }
    }

   // Try built-in optimizations 
    for (Opts = DefaultOptimization;
         Opts != NULL;
         Opts = Opts ->Next) {

            if (Opts ->OptimizePtr(PtrLut, Intent, InputFormat, OutputFormat, dwFlags)) {

                return TRUE;  
            }
    }

    // Only simple optimizations succeeded
    return AnySuccess;
}