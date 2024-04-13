oftrace_node_print_details(struct ds *output,
                           const struct ovs_list *nodes, int level)
{
    const struct oftrace_node *sub;
    LIST_FOR_EACH (sub, node, nodes) {
        if (sub->type == OFT_BRIDGE) {
            ds_put_char(output, '\n');
        }

        bool more = (sub->node.next != nodes
                     || oftrace_node_type_is_terminal(sub->type));

        ds_put_char_multiple(output, ' ', (level + more) * 4);
        switch (sub->type) {
        case OFT_DETAIL:
            ds_put_format(output, " -> %s\n", sub->text);
            break;
        case OFT_WARN:
            ds_put_format(output, " >> %s\n", sub->text);
            break;
        case OFT_ERROR:
            ds_put_format(output, " >>>> %s <<<<\n", sub->text);
            break;
        case OFT_BRIDGE:
            ds_put_format(output, "%s\n", sub->text);
            ds_put_char_multiple(output, ' ', (level + more) * 4);
            ds_put_char_multiple(output, '-', strlen(sub->text));
            ds_put_char(output, '\n');
            break;
        case OFT_TABLE:
        case OFT_BUCKET:
        case OFT_THAW:
        case OFT_ACTION:
            ds_put_format(output, "%s\n", sub->text);
            break;
        }

        oftrace_node_print_details(output, &sub->subs, level + more + more);
    }
}