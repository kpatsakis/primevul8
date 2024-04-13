ofproto_dpif_trace_init(void)
{
    static bool registered;
    if (registered) {
        return;
    }
    registered = true;

    unixctl_command_register(
        "ofproto/trace",
        "{[dp_name] odp_flow | bridge br_flow} [OPTIONS...] "
        "[-generate|packet]", 1, INT_MAX, ofproto_unixctl_trace, NULL);
    unixctl_command_register(
        "ofproto/trace-packet-out",
        "[-consistent] {[dp_name] odp_flow | bridge br_flow} [OPTIONS...] "
        "[-generate|packet] actions",
        2, INT_MAX, ofproto_unixctl_trace_actions, NULL);
}