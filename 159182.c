explain_slow_path(enum slow_path_reason slow, struct ds *output)
{
    ds_put_cstr(output, "\nThis flow is handled by the userspace "
                "slow path because it:");
    for (; slow; slow = zero_rightmost_1bit(slow)) {
        enum slow_path_reason bit = rightmost_1bit(slow);
        ds_put_format(output, "\n  - %s.",
                      slow_path_reason_to_explanation(bit));
    }
}