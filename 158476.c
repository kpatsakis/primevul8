debug_print_argv(const uschar ** argv)
{
debug_printf("exec");
while (*argv) debug_printf(" %.256s", *argv++);
debug_printf("\n");
}