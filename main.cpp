/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#include <string>
#include "X11/Xlib.h"

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *image = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The thread that will be used
SDL_Thread *thread = NULL;

//Quit flag
bool quit = false;

SDL_Surface* personaje_up[2];
SDL_Surface* personaje_down[2];
SDL_Surface* personaje_left[2];
SDL_Surface* personaje_right[2];

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    XInitThreads();
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Thread test", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the image
    image = load_image( "image.png" );

    //If there was an error in loading the image
    if( image == NULL )
    {
        return false;
    }

    personaje_down[0]=load_image("personaje/down1.png");
    personaje_down[1]=load_image("personaje/down2.png");
    personaje_up[0]=load_image("personaje/up1.png");
    personaje_up[1]=load_image("personaje/up2.png");
    personaje_left[0]=load_image("personaje/left1.png");
    personaje_left[1]=load_image("personaje/left2.png");
    personaje_right[0]=load_image("personaje/right1.png");
    personaje_right[1]=load_image("personaje/right2.png");

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Stop the thread
    SDL_KillThread( thread );

    //Free the surface
    SDL_FreeSurface( image );

    //Quit SDL
    SDL_Quit();
}

int image_x=0;
int image_y=0;

int my_thread( void *data )
{
    //While the program is not over
    while( quit == false )
    {
        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }

        Uint8 *keystates = SDL_GetKeyState( NULL );

        if( keystates[ SDLK_UP ] )
        {
            image_y--;
        }
        if( keystates[ SDLK_DOWN ] )
        {
            image_y++;
        }
        if( keystates[ SDLK_RIGHT ] )
        {
            image_x++;
        }
        if( keystates[ SDLK_LEFT ] )
        {
            image_x--;
        }

        SDL_Delay( 15 );
    }

    return 0;
}

int main( int argc, char* args[] )
{
    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Create and run the thread
    thread = SDL_CreateThread( my_thread, NULL );

    //While the user hasn't quit
    while( quit == false )
    {
        //Apply the image to the screen
        apply_surface( image_x, image_y, personaje_down[0], screen );

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }
    }

    //Clean up
    clean_up();

    return 0;
}
