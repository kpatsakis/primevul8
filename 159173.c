oftrace_node_type_is_terminal(enum oftrace_node_type type)
{
    switch (type) {
    case OFT_ACTION:
    case OFT_DETAIL:
    case OFT_WARN:
    case OFT_ERROR:
    case OFT_BUCKET:
        return true;

    case OFT_BRIDGE:
    case OFT_TABLE:
    case OFT_THAW:
        return false;
    }

    OVS_NOT_REACHED();
}