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
	GUI *g;
	TransScreen *ts;

	NOT(e);

	g = &e->gui;
	ts = &g->transScreen;
	
	switch (g->focus) {
	case guiFocusTitle: {
		if (g->next == guiFocusTransScreen && ts->next == guiFocusMenu) {
			Mix_PlayChannel(audioChanSfx, e->io.openSnd, 0); 
		}
		break;
	}
	case guiFocusGameGUI: {
		switch (e->gui.gameGui.validPlay) {
		case yes: Mix_PlayChannel(audioChanSfx, e->io.correctSnd, 0); break;
		case no: Mix_PlayChannel(audioChanSfx, e->io.incorrectSnd, 0); break;
		default: break;
		}
		if (g->next == guiFocusGameMenu) {
			Mix_PlayChannel(audioChanSfx, e->io.pauseSnd, 0); 
			Mix_PauseMusic();
		}
		break;
	}
	case guiFocusGameMenu: {
		if (g->next == guiFocusGameGUI) {
			Mix_PlayChannel(audioChanSfx, e->io.enterSnd, 0); 
			Mix_ResumeMusic();
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
	case guiFocusTransScreen: {
		if (g->next == guiFocusGameGUI) {
			/*Mix_PlayMusic(e->io.gameSong, -1);*/
		}
		break;
	}
	default: break;
	}
}

