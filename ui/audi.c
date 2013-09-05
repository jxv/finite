#include <math.h>

#include "gui.h"
#include "audi.h"
#include "init.h"
#include "widget.h"
#include "print.h"

void audiMenuWidget(MenuWidget *mv, Mix_Chunk *mc)
{
	if (mv->focus != mv->next) {
		Mix_PlayChannel(audioChanSfx, mc, 0); 
	}
}

void audi_guiFocusTitle(Env *e)
{
	GUI *g;
	TransScreen *ts;

	NOT(e);

	g = &e->gui;
	ts = &g->transScreen;

	if (g->next == guiFocusTransScreen && ts->next == guiFocusMenu) {
		Mix_PlayChannel(audioChanSfx, e->io.openSnd, 0); 
	}
}

void audi_guiFocusSettings(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;

	audiMenuWidget(&g->settings.menu, e->io.scrollSnd);
	switch (g->settings.menu.focus) {
		case settingsFocusSfx: {
			int i;
			for (i = 0; i < audioChanCount; i++) {
				Mix_Volume(i, g->settings.vol[volSfx] * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
			}
		}
		case settingsFocusMusic: {
			Mix_VolumeMusic(g->settings.vol[volMus] * MIX_MAX_VOLUME / MAX_GUI_VOLUME);
		}
	}
}

void audi_guiFocusMenu(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;
	audiMenuWidget(&g->menu, e->io.scrollSnd);
}

void audi_guiFocusGameGUI(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;

	switch (g->gameGui.validPlay) {
	case yes: Mix_PlayChannel(audioChanSfx, e->io.correctSnd, 0); break;
	case no: Mix_PlayChannel(audioChanSfx, e->io.incorrectSnd, 0); break;
	default: break;
	}
	if (g->next == guiFocusGameMenu) {
		Mix_PlayChannel(audioChanSfx, e->io.pauseSnd, 0); 
		Mix_PauseMusic();
	}
}

void audi_guiFocusGameMenu(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;

	audiMenuWidget(&g->gameMenu, e->io.scrollSnd);
	if (g->next == guiFocusGameGUI) {
		Mix_PlayChannel(audioChanSfx, e->io.enterSnd, 0); 
		Mix_ResumeMusic();
	}

}

void audi_guiFocusGameAreYouSureQuit(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;
	audiMenuWidget(&g->gameAreYouSureQuit, e->io.scrollSnd);
}

void audi_guiFocusPlayMenu(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;
	audiMenuWidget(&g->playMenu, e->io.scrollSnd);
}

void audi_guiFocusOptions(Env *e)
{
	GUI *g;

	NOT(e);
	
	g = &e->gui;
	audiMenuWidget(&g->options.menu, e->io.scrollSnd);
}

void audi_guiFocusControls(Env *e)
{
	GUI *g;

	NOT(e);

	g = &e->gui;

	audiMenuWidget(&g->controlsMenu.menu, e->io.scrollSnd);
	if (g->controlsMenu.dups && g->controlsMenu.time == 1.0f) {
		Mix_PlayChannel(audioChanSfx, e->io.incorrectSnd, 0); 
	}
}

void audi_guiFocusTransScreen(Env *e)
{
	GUI *g;

	NOT(e);

	g = &e->gui;

	if (g->next == guiFocusGameGUI) {
		/*Mix_PlayMusic(e->io.gameSong, -1);*/
	}
}

void audi(Env *e)
{
	GUI *g;

	NOT(e);

	g = &e->gui;
	
	switch (g->focus) {
	case guiFocusTitle: audi_guiFocusTitle(e); break;
	case guiFocusMenu: audi_guiFocusMenu(e); break;
	case guiFocusGameGUI: audi_guiFocusGameGUI(e); break;
	case guiFocusGameMenu: audi_guiFocusGameMenu(e); break;
	case guiFocusPlayMenu: audi_guiFocusPlayMenu(e); break;
	case guiFocusSettings: audi_guiFocusSettings(e); break;
	case guiFocusControls: audi_guiFocusControls(e); break;
	case guiFocusOptions: audi_guiFocusOptions(e); break;
	case guiFocusGameAreYouSureQuit: audi_guiFocusGameAreYouSureQuit(e); break;
	case guiFocusTransScreen: audi_guiFocusTransScreen(e); break;
	default: break;
	}
}

