Ta3Grammar_LabelRepr(label *lb)
{
    static char buf[100];

    if (lb->lb_type == ENDMARKER)
        return "EMPTY";
    else if (ISNONTERMINAL(lb->lb_type)) {
        if (lb->lb_str == NULL) {
            PyOS_snprintf(buf, sizeof(buf), "NT%d", lb->lb_type);
            return buf;
        }
        else
            return lb->lb_str;
    }
    else if (lb->lb_type < N_TOKENS) {
        if (lb->lb_str == NULL)
            return _Ta3Parser_TokenNames[lb->lb_type];
        else {
            PyOS_snprintf(buf, sizeof(buf), "%.32s(%.32s)",
                _Ta3Parser_TokenNames[lb->lb_type], lb->lb_str);
            return buf;
        }
    }
    else {
        Py_FatalError("invalid label");
        return NULL;
    }
}