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
	case guiFocusGameGUI: {
		switch (e->gui.gameGui.validPlay) {
		case yes: Mix_PlayChannel(audioChanMusic, e->io.correctSnd, 0); break;
		case no: Mix_PlayChannel(audioChanSfx, e->io.incorrectSnd, 0); break;
		default: break;
		}
		break;
	}
	case guiFocusSettings: {
		Mix_Volume(audioChanMusic, e->gui.settings.musVolume * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		Mix_Volume(audioChanSfx, e->gui.settings.sfxVolume * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		break;
	}
	default: break;
	}
}

