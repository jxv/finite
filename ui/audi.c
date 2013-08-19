#include <math.h>

#include "gui.h"
#include "audi.h"
#include "init.h"
#include "widget.h"
#include "print.h"

void audi(Env *e)
{
	NOT(e);
	
	switch (e->gui.focus) {
	case GUI_FOCUS_GAME_GUI: {
		switch (e->gui.gameGui.validPlay) {
		case YES: Mix_PlayChannel(AUDIO_CHAN_SFX, e->io.correctSnd, 0); break;
		case NO: Mix_PlayChannel(AUDIO_CHAN_SFX, e->io.incorrectSnd, 0); break;
		default: break;
		}
		break;
	}
	case GUI_FOCUS_SETTINGS: {
		Mix_Volume(AUDIO_CHAN_MUSIC, e->gui.settings.musVolume * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		Mix_Volume(AUDIO_CHAN_SFX, e->gui.settings.sfxVolume * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		break;
	}
	default: break;
	}
}

