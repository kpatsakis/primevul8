NodeDebugInfo::NodeDebugInfo(const NodeDef& ndef)
    : NodeDebugInfo(ndef.name(), ndef.has_experimental_debug_info(),
                    ndef.experimental_debug_info()) {}