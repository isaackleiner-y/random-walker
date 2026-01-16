// gcc -IC:\SDL\x86_64-w64-mingw32\include -LC:\SDL\x86_64-w64-mingw32\lib rWalk.c -lSDL3 -o rWalk.exe

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1920
#define HEIGHT 1080
#define AGENT_SIZE 3

int scale = 10;

typedef struct {
    float r, g, b;
} RGB;

typedef struct{
    int vx, vy;
}   Velocity;

typedef struct{
    int x,y;
    RGB color;
}   Agent;

Velocity get_rand_v()
{
    int choice = rand() % 4;
    switch(choice){
        case 0: // up
            return (Velocity) {0, -1};
            break;
        case 1: // down
            return (Velocity) {0, 1};
            break;
        case 2: // right
            return (Velocity) {1, 0};
            break;
        case 3: // left
            return (Velocity) {-1, 0};
            break;
    } 
    fprintf(stderr, "IMPOSSIBLE RANDOM VALUE %d ENCOUNTERED\n", choice);
    exit(-1);
}

float hue2rgb(float p, float q, float t) {
    if (t < 0)
        t += 1;
    if (t > 1)
        t -= 1;
    if (t < 1./6)
        return p + (q - p) * 6 * t;
    if (t < 2./3)
        return p + (q - p) * (2./3 - t) * 6;

    return p;
}

// Я хз как работает эта фукнция xdd
RGB hsl2rgb(float h, float s, float l)
{
    RGB result;

    if (0 == s) {
        result.r = result.g = result.b = l * 255;
    }
    else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        result.r = hue2rgb(p,q, h + 1./3) * 255;
        result.g = hue2rgb(p, q, h) * 255;
        result.b = hue2rgb(p, q, h - 1./3) * 255;
    }
    return result;
}

void create_agents(Agent *pAgents, int num_agents)
{
    for (int i = 0; i<num_agents; i++)
    {
        float h = ((float) rand() / (float) RAND_MAX);
        
        RGB rgb = hsl2rgb(h, 1, 0.5);
        pAgents[i] = (Agent) {WIDTH/2, HEIGHT/2, rgb};
    }
}

void move_agent(SDL_Surface *pSurface, Agent *pAgent)
{
    
    Velocity v = get_rand_v();
    RGB rgb = pAgent->color;

    for (int i = 0; i < scale; i++)
    {
        pAgent->x += v.vx;
        pAgent->y += v.vy;
        SDL_Rect rect = (SDL_Rect) {pAgent->x, pAgent->y, AGENT_SIZE, AGENT_SIZE};

        // Get SDL Color
        Uint32 color = SDL_MapSurfaceRGB(pSurface, rgb.r, rgb.g, rgb.b);
        SDL_FillSurfaceRect(pSurface, &rect, color);
    }
}

int main(int argc, const char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    int windowCord = SDL_WINDOWPOS_CENTERED;
    int num_agents;
    if (argc == 1)
    {
        num_agents = 5;
    }
    else if (argc == 2)
    {
        num_agents = atoi(argv[1]);
    }
    else
    {
        printf("Usage:  %s <num-of-the-agents>\n", argv[0]);
        return -1;
    }

    srand(time(NULL));

    // 

    SDL_Window *pWindow = SDL_CreateWindow("rWalk", WIDTH, HEIGHT, 0);
    SDL_Surface *pSurface = SDL_GetWindowSurface(pWindow);

    Agent *pAgents = calloc(num_agents, sizeof(Agent));
    create_agents(pAgents, num_agents);

    bool app_running = true;
    bool is_pause = false;

    while(app_running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                app_running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.scancode == SDL_SCANCODE_Q)
                    app_running = false;
                else if (event.key.scancode == SDL_SCANCODE_SPACE)
                    is_pause = !is_pause;
                    break;
                
                break;

            default:
                break;
            }
        }

        if (!is_pause)
        {
            for (int i = 0; i < num_agents; i++)
            {
                move_agent(pSurface, &pAgents[i]);
            }
        }


        
        SDL_UpdateWindowSurface(pWindow);
        SDL_Delay(10);
    }
    
    free(pAgents);

    return 0;
}