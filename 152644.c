int get_format_position(struct report_format *fmt[], unsigned int format)
{
        int i;

        for (i = 0; i < NR_FMT; i++) {
                if (fmt[i]->id == format)
                        break;
        }

        if (i == NR_FMT)
		/* Should never happen */
                return 0;

        return i;
}