ChangeKbdFeedback(ClientPtr client, DeviceIntPtr dev, long unsigned int mask,
                  KbdFeedbackPtr k, xKbdFeedbackCtl * f)
{
    KeybdCtrl kctrl;
    int t;
    int key = DO_ALL;

    if (client->swapped) {
        swaps(&f->length);
        swaps(&f->pitch);
        swaps(&f->duration);
        swapl(&f->led_mask);
        swapl(&f->led_values);
    }

    kctrl = k->ctrl;
    if (mask & DvKeyClickPercent) {
        t = f->click;
        if (t == -1)
            t = defaultKeyboardControl.click;
        else if (t < 0 || t > 100) {
            client->errorValue = t;
            return BadValue;
        }
        kctrl.click = t;
    }

    if (mask & DvPercent) {
        t = f->percent;
        if (t == -1)
            t = defaultKeyboardControl.bell;
        else if (t < 0 || t > 100) {
            client->errorValue = t;
            return BadValue;
        }
        kctrl.bell = t;
    }

    if (mask & DvPitch) {
        t = f->pitch;
        if (t == -1)
            t = defaultKeyboardControl.bell_pitch;
        else if (t < 0) {
            client->errorValue = t;
            return BadValue;
        }
        kctrl.bell_pitch = t;
    }

    if (mask & DvDuration) {
        t = f->duration;
        if (t == -1)
            t = defaultKeyboardControl.bell_duration;
        else if (t < 0) {
            client->errorValue = t;
            return BadValue;
        }
        kctrl.bell_duration = t;
    }

    if (mask & DvLed) {
        kctrl.leds &= ~(f->led_mask);
        kctrl.leds |= (f->led_mask & f->led_values);
    }

    if (mask & DvKey) {
        key = (KeyCode) f->key;
        if (key < 8 || key > 255) {
            client->errorValue = key;
            return BadValue;
        }
        if (!(mask & DvAutoRepeatMode))
            return BadMatch;
    }

    if (mask & DvAutoRepeatMode) {
        int inx = (key >> 3);
        int kmask = (1 << (key & 7));

        t = (CARD8) f->auto_repeat_mode;
        if (t == AutoRepeatModeOff) {
            if (key == DO_ALL)
                kctrl.autoRepeat = FALSE;
            else
                kctrl.autoRepeats[inx] &= ~kmask;
        }
        else if (t == AutoRepeatModeOn) {
            if (key == DO_ALL)
                kctrl.autoRepeat = TRUE;
            else
                kctrl.autoRepeats[inx] |= kmask;
        }
        else if (t == AutoRepeatModeDefault) {
            if (key == DO_ALL)
                kctrl.autoRepeat = defaultKeyboardControl.autoRepeat;
            else
                kctrl.autoRepeats[inx] &= ~kmask;
            kctrl.autoRepeats[inx] =
                (kctrl.autoRepeats[inx] & ~kmask) |
                (defaultKeyboardControl.autoRepeats[inx] & kmask);
        }
        else {
            client->errorValue = t;
            return BadValue;
        }
    }

    k->ctrl = kctrl;
    (*k->CtrlProc) (dev, &k->ctrl);
    return Success;
}