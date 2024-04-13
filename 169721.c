perf_aux_output_match(struct perf_event *event, struct perf_event *aux_event)
{
	if (!has_aux(aux_event))
		return 0;

	if (!event->pmu->aux_output_match)
		return 0;

	return event->pmu->aux_output_match(aux_event);
}