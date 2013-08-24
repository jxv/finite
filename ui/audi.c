#include <math.h>

#include "gui.h"
#include "audi.h"
#include "init.h"
#include "widget.h"
#include "print.h"

void audi_guiFocusSettings(Env *e)
{
	NOT(e);
	switch (e->gui.settings.menu.focus) {
		case settingsFocusSfx: {
			int i;
			for (i = 0; i < audioChanCount; i++) {
				Mix_Volume(i, e->gui.settings.vol[volSfx] * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
			}
		}
		case settingsFocusMusic: {
			Mix_VolumeMusic(e->gui.settings.vol[volMus] * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		}
	}
}

void audi(Env *e)
{
	NOT(e);
	
	switch (e->gui.focus) {
	case guiFocusGameGUI: {
		switch (e->gui.gameGui.validPlay) {
		case yes: Mix_PlayChannel(audioChanSfx, e->io.correctSnd, 0); break;
		case no: Mix_PlayChannel(audioChanSfx, e->io.incorrectSnd, 0); break;
		default: break;
		}
		break;
	}
	case guiFocusSettings: audi_guiFocusSettings(e); break;
	case guiFocusControls: {
		if (e->gui.controlsMenu.dups && e->gui.controlsMenu.time == 1.0f) {
			Mix_PlayChannel(audioChanSfx, e->io.incorrectSnd, 0); 
		}
		break;
	}
	default: break;
	}
}

