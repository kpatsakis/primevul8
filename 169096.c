long long ass_step_sub(ASS_Track *track, long long now, int movement)
{
    int i;
    ASS_Event *best = NULL;
    long long target = now;
    int direction = (movement > 0 ? 1 : -1) * !!movement;

    if (track->n_events == 0)
        return 0;

    do {
        ASS_Event *closest = NULL;
        long long closest_time = now;
        for (i = 0; i < track->n_events; i++) {
            if (direction < 0) {
                long long end =
                    track->events[i].Start + track->events[i].Duration;
                if (end < target) {
                    if (!closest || end > closest_time) {
                        closest = &track->events[i];
                        closest_time = end;
                    }
                }
            } else if (direction > 0) {
                long long start = track->events[i].Start;
                if (start > target) {
                    if (!closest || start < closest_time) {
                        closest = &track->events[i];
                        closest_time = start;
                    }
                }
            } else {
                long long start = track->events[i].Start;
                if (start < target) {
                    if (!closest || start >= closest_time) {
                        closest = &track->events[i];
                        closest_time = start;
                    }
                }
            }
        }
        target = closest_time + direction;
        movement -= direction;
        if (closest)
            best = closest;
    } while (movement);

    return best ? best->Start - now : 0;
}