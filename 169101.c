void ass_lazy_track_init(ASS_Library *lib, ASS_Track *track)
{
    if (track->PlayResX > 0 && track->PlayResY > 0)
        return;
    if (track->PlayResX <= 0 && track->PlayResY <= 0) {
        ass_msg(lib, MSGL_WARN,
               "Neither PlayResX nor PlayResY defined. Assuming 384x288");
        track->PlayResX = 384;
        track->PlayResY = 288;
    } else {
        if (track->PlayResY <= 0 && track->PlayResX == 1280) {
            track->PlayResY = 1024;
            ass_msg(lib, MSGL_WARN,
                   "PlayResY undefined, setting to %d", track->PlayResY);
        } else if (track->PlayResY <= 0) {
            track->PlayResY = FFMAX(1, track->PlayResX * 3LL / 4);
            ass_msg(lib, MSGL_WARN,
                   "PlayResY undefined, setting to %d", track->PlayResY);
        } else if (track->PlayResX <= 0 && track->PlayResY == 1024) {
            track->PlayResX = 1280;
            ass_msg(lib, MSGL_WARN,
                   "PlayResX undefined, setting to %d", track->PlayResX);
        } else if (track->PlayResX <= 0) {
            track->PlayResX = FFMAX(1, track->PlayResY * 4LL / 3);
            ass_msg(lib, MSGL_WARN,
                   "PlayResX undefined, setting to %d", track->PlayResX);
        }
    }
}