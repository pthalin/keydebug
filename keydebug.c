#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include "clacon.ttf.h"


const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int SCREEN_BPP = 16;

char tmp[64] = "";

SDL_Surface *screen = NULL;
SDL_Surface *message = NULL;
SDL_Surface *info = NULL;
SDL_Surface *background = NULL;
SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = { 255, 255, 255 };

char* key_name(int sym) {

  switch (sym) {
  case   8: return "BACKSPACE";
  case   9: return "TAB";
  case  13: return "RETURN";
  case  27: return "ESCAPE";
  case  32: return "SPACE";
  case 303: return "RSHIFT";
  case 304: return "LSHIFT";
  case 305: return "RCTRL";
  case 306: return "LCTRL";
  case 307: return "RALT";
  case 308: return "LALT";
  case 273: return "UP";
  case 274: return "DOWN";
  case 275: return "RIGHT";
  case 276: return "LEFT";
  default:  return "?";
 }
}

bool init()
{
    if(SDL_Init( SDL_INIT_EVERYTHING ) == -1) {
      return false;
    }
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
			      SCREEN_BPP, SDL_SWSURFACE);


    if(screen == NULL) {
      return false;
    }
    
    SDL_ShowCursor(SDL_DISABLE);
    
    if(TTF_Init() == -1) {
      return false;
    }

    //font = TTF_OpenFont("clacon.ttf", 26 );
    font = TTF_OpenFontRW(SDL_RWFromMem((void *) clacon_ttf, sizeof(clacon_ttf)), 1, 26);
    if(font == NULL) {
      return false;
    }

    background = SDL_CreateRGBSurface(SDL_SWSURFACE,
				      SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
				      screen->format->Rmask,
				      screen->format->Gmask,
				      screen->format->Bmask,
				      screen->format->Amask);
    if(background == NULL) {
      return false;
    }

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = SCREEN_WIDTH;
    rect.h = SCREEN_HEIGHT;
    SDL_FillRect(background, &rect, 0);

    info = TTF_RenderText_Solid( font, "Press a key 3 times to exit" , textColor );

    rect.x = 0;
    rect.y = 200;
    rect.w = 0;
    rect.h = 0;

    SDL_BlitSurface(info, NULL, background, &rect );
    
    return true;
}

void clean_up()
{
    TTF_CloseFont( font );
    TTF_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    bool quit = false;
    int repkey = 0;
    int prevkey = 0;
    if( init() == false ) {
      printf("init() failed\n");
      return 1;
    }

    SDL_BlitSurface(background, NULL, screen, NULL );
    
    while( quit == false ) {
      if( SDL_PollEvent( &event ) ) {
	if( event.type == SDL_KEYDOWN ) {
	  int keysym = event.key.keysym.sym;
	  sprintf(tmp, "KeySym: %d %s", keysym, key_name(keysym));
	  
	  message =  TTF_RenderText_Solid( font, tmp , textColor );
	  if ( prevkey == keysym)
	    repkey++;
	  else
	    repkey = 0;
	  prevkey = keysym;
	}

	else if( repkey > 2 ) {
	  quit = true;
	}
      }

      if( message != NULL ) {
	SDL_BlitSurface(background, NULL, screen, NULL );
	SDL_BlitSurface(message, NULL, screen, NULL );
	SDL_FreeSurface( message );
	message = NULL;
      }
      
      if( SDL_Flip( screen ) == -1 ) {
	return 1;
      }
    }

    clean_up();

    return 0;
}

