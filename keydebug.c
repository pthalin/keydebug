#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "clacon.ttf.h"


const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int SCREEN_BPP = 16;

char device[64] = "";
char hwrev[64] = "";

SDL_Surface *screen = NULL;
SDL_Surface *message = NULL;
SDL_Surface *info = NULL;
SDL_Surface *background = NULL;
SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = { 255, 255, 255 };

typedef enum {BITTBOY35, BITTBOY3, BITTBOY2, POCKETGO, V90, Q90, V90orQ90, UNKNOWN} t_devid;
t_devid devid = UNKNOWN;

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

void get_device()
{ 
  int len = 0;
  char *endofstring = NULL;
  char* pos = NULL;


  FILE *fp = NULL;
  fp = fopen("/etc/cfw-info", "r");
  if(fp != NULL) {
    while(!feof(fp)){
      char tmp[64] = "";
      fgets(tmp, 64, fp);
      len = strlen(tmp);
    
      if (tmp[0] == '#') {
        continue;
      }
      if (NULL != (endofstring = strchr(tmp, '\r'))) {
        len = endofstring-tmp;
      }
      else if (NULL != (endofstring = strchr(tmp, '\n'))) {
        len = endofstring-tmp;
      }
      if (NULL == (pos = strchr(tmp, '='))) {
        continue;
      }

      char key[64] = "";
      char val[64] = "";
      strncpy(key, tmp, pos - tmp);
      strncpy(val, pos + 1, tmp + len - pos-1);

      printf("%s -> %s\n", key, val);
      if (0==strcmp(key, "DEVICE_NAME")) {
        strcpy(device, val);
      }
      if (0==strcmp(key, "DEVICE_REV")) {
        strcpy(hwrev, val);
      }
  }
  if (0==strcmp(device, "BITTBOY")) {
     if (0==strcmp(hwrev, "3.5")) {
        devid = BITTBOY35;
      }
      else if (0==strcmp(hwrev, "3")) {
        devid = BITTBOY3;
      }
      else if (0==strcmp(hwrev, "2")) {
        devid = BITTBOY2;
      }
      else {
        devid = UNKNOWN;
      }
  } else if (0==strcmp(device, "POCKETGO")) {
    devid = POCKETGO;
  } else if (0==strcmp(device, "V90Q90")) {
    devid = V90orQ90;
  }
  else{
   devid = UNKNOWN;
  }
  
  fclose(fp);
  }
}

int init()
{
    SDL_Rect rect;

    get_device();

    if(SDL_Init(SDL_INIT_EVERYTHING ) == -1) {
      return -1;
    }
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);


    if(screen == NULL) {
      return -2;
    }
    
    SDL_ShowCursor(SDL_DISABLE);
    
    if(TTF_Init() == -1) {
      return -3;
    }

    font = TTF_OpenFontRW(SDL_RWFromMem((void *) clacon_ttf, sizeof(clacon_ttf)), 1, 26);
    if(font == NULL) {
      return -4;
    }

    background = SDL_CreateRGBSurface(SDL_SWSURFACE,
				      SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
				      screen->format->Rmask,
				      screen->format->Gmask,
				      screen->format->Bmask,
				      screen->format->Amask);
    if(background == NULL) {
      return -5;
    }

    rect.x = 0;
    rect.y = 0;
    rect.w = SCREEN_WIDTH;
    rect.h = SCREEN_HEIGHT;
    SDL_FillRect(background, &rect, 0);

    info = TTF_RenderText_Solid(font, "Press a key 3 times to exit" , textColor);
    rect.x = 10;
    rect.y = 200;
    rect.w = 0;
    rect.h = 0;
    if(info != NULL) {
      SDL_BlitSurface(info, NULL, background, &rect);
	    SDL_FreeSurface(info);
	    info = NULL;
    }
    else {
      return -6;
    }

    info = TTF_RenderText_Solid(font, device , textColor);
    rect.x = 10;
    rect.y = 160;
    rect.w = 0;
    rect.h = 0;
    if(info != NULL) {
      SDL_BlitSurface(info, NULL, background, &rect);
	    SDL_FreeSurface(info);
	    info = NULL;
    }
    else {
      return -6;
    }

    info = TTF_RenderText_Solid(font, hwrev , textColor);
    rect.x = 10;
    rect.y = 180;
    rect.w = 0;
    rect.h = 0;
    if(info != NULL) {
      SDL_BlitSurface(info, NULL, background, &rect);
	    SDL_FreeSurface(info);
	    info = NULL;
    }
    else {
      return -6;
    }



    return 0;
}

void clean_up()
{
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* args[] )
{
    int quit = 0;
    int repkey = 0;
    int prevkey = 0;
    int keysym = 0;
    int error = 0;
    char tmp[64] = "";

    if((error = init()) != 0) {
      printf("init() failed with code %d\n", error);
      return 1;
    }

    SDL_BlitSurface(background, NULL, screen, NULL);
    
    while(quit != 1) {
      if(SDL_PollEvent(&event)) {
	      if(event.type == SDL_KEYDOWN) {
	        keysym = event.key.keysym.sym;
	        sprintf(tmp, " KeySym: %d (%s)", keysym, key_name(keysym));
	        message =  TTF_RenderText_Solid(font, tmp , textColor);
          if (message == NULL) {
            break;
          }
	        if (prevkey == keysym) {
	          repkey++;
          }
	        else {
	          repkey = 0;
          }
	        prevkey = keysym;
	    }
    	else if(repkey > 2) {
	      quit = 1;
	    }
    }

    if(message != NULL) {
	    SDL_BlitSurface(background, NULL, screen, NULL);
	    SDL_BlitSurface(message, NULL, screen, NULL);
	    SDL_FreeSurface(message);
	    message = NULL;
      tmp[0] = 0;

      switch(devid)
      {
      case BITTBOY2:
      case BITTBOY3:
      case BITTBOY35:
        switch(keysym)
        {
          case SDLK_LCTRL:
            sprintf(tmp, "%s", "Button: A");
            break;
          case SDLK_SPACE:
            sprintf(tmp, "%s", "Button: B");
            break;
          case SDLK_LALT:
            sprintf(tmp, "%s", "Button: TA");
            break;
          case SDLK_LSHIFT:
            sprintf(tmp, "%s", "Button: TB");
            break;
        } 
        break;

      case POCKETGO:
      case Q90:
      case V90:
      case V90orQ90:
        switch(keysym)
        {
          case SDLK_LALT:
            sprintf(tmp, "%s", "Button: A");
            break;
          case SDLK_LCTRL:
            sprintf(tmp, "%s", "Button: B");
            break;
          case SDLK_LSHIFT:
            sprintf(tmp, "%s", "Button: X");
            break;
          case SDLK_SPACE:
            sprintf(tmp, "%s", "Button: Y");
            break;
        } 
        break;
      
      default:
        break;
      }

      switch(keysym)
      {
        case SDLK_ESCAPE:
          sprintf(tmp, "%s", "Button: SELECT");
          break;
        case SDLK_RETURN:
          sprintf(tmp, "%s", "Button: START");
          break;
        case SDLK_RCTRL:
          sprintf(tmp, "%s", "Button: R (Reset/Menu)");
          break;
        case SDLK_BACKSPACE:
          sprintf(tmp, "%s", "Button: R1");
          break;
        case SDLK_TAB:
          sprintf(tmp, "%s", "Button: L1");
          break;
        case SDLK_RSHIFT:
          sprintf(tmp, "%s", "Button: R2");
          break;
        case SDLK_RALT:
          sprintf(tmp, "%s", "Button: L2");
          break;
        case SDLK_UP:
          sprintf(tmp, "%s", "Button: Up");
          break;
        case SDLK_DOWN:
          sprintf(tmp, "%s", "Button: Down");
          break;
        case SDLK_RIGHT:
          sprintf(tmp, "%s", "Button: Right");
          break;
        case SDLK_LEFT:
          sprintf(tmp, "%s", "Button: Left");
          break;


      }

      if (tmp[0] == 0) {
        sprintf(tmp, "%s", "Button: Unkown");
      }


      SDL_Rect rect;
      rect.x = 10;
      rect.y = 30;
      rect.w = 0;
      rect.h = 20;
      message =  TTF_RenderText_Solid(font, tmp , textColor);
      if(message != NULL) {
        SDL_BlitSurface(message, NULL, screen,  &rect);
        SDL_FreeSurface(message);
        message = NULL;
      }
    }


      
    if(SDL_Flip(screen) == -1) {
	    return 1;
    }
  }
  clean_up();

  return 0;
}

