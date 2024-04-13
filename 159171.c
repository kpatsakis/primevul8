prune_output_actions(const struct ofpbuf *in, struct ofpbuf *out)
{
    const struct nlattr *a;
    unsigned int left;
    NL_ATTR_FOR_EACH (a, left, in->data, in->size) {
        if (a->nla_type == OVS_ACTION_ATTR_CLONE) {
            struct ofpbuf in_nested;
            nl_attr_get_nested(a, &in_nested);

            size_t ofs = nl_msg_start_nested(out, OVS_ACTION_ATTR_CLONE);
            prune_output_actions(&in_nested, out);
            nl_msg_end_nested(out, ofs);
        } else if (a->nla_type != OVS_ACTION_ATTR_OUTPUT &&
                   a->nla_type != OVS_ACTION_ATTR_RECIRC) {
            ofpbuf_put(out, a, NLA_ALIGN(a->nla_len));
        }
    }
}