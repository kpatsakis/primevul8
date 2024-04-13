cmsBool  _cmsRegisterOptimizationPlugin(cmsContext ContextID, cmsPluginBase* Data)
{
    cmsPluginOptimization* Plugin = (cmsPluginOptimization*) Data;
    _cmsOptimizationPluginChunkType* ctx = ( _cmsOptimizationPluginChunkType*) _cmsContextGetClientChunk(ContextID, OptimizationPlugin);
    _cmsOptimizationCollection* fl;

    if (Data == NULL) {

        ctx->OptimizationCollection = NULL;
        return TRUE;
    }

    // Optimizer callback is required
    if (Plugin ->OptimizePtr == NULL) return FALSE;

    fl = (_cmsOptimizationCollection*) _cmsPluginMalloc(ContextID, sizeof(_cmsOptimizationCollection));
    if (fl == NULL) return FALSE;

    // Copy the parameters
    fl ->OptimizePtr = Plugin ->OptimizePtr;

    // Keep linked list
    fl ->Next = ctx->OptimizationCollection;

    // Set the head
    ctx ->OptimizationCollection = fl;

    // All is ok
    return TRUE;
}