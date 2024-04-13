void ass_free_event(ASS_Track *track, int eid)
{
    ASS_Event *event = track->events + eid;

    free(event->Name);
    free(event->Effect);
    free(event->Text);
    free(event->render_priv);
}