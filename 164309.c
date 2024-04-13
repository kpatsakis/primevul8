collector_strimwidth(int c, void* data)
{
	struct collector_strimwidth_data *pc = (struct collector_strimwidth_data*)data;

	switch (pc->status) {
	case 10:
		(*pc->decoder->filter_function)(c, pc->decoder);
		break;
	default:
		if (pc->outchar >= pc->from) {
			pc->outwidth += (is_fullwidth(c) ? 2: 1);

			if (pc->outwidth > pc->width) {
				if (pc->status == 0) {
					pc->endpos = pc->device.pos;
					mbfl_convert_filter_copy(pc->decoder, pc->decoder_backup);
				}
				pc->status++;
				(*pc->decoder->filter_function)(c, pc->decoder);
				c = -1;
			} else {
				(*pc->decoder->filter_function)(c, pc->decoder);
			}
		}
		pc->outchar++;
		break;
	}

	return c;
}