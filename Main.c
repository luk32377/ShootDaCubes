

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define true 1
#define false 0
typedef char bool;

typedef struct {
    int max;
    int current;
    void** data;
} Vector;

Vector* VectorCreate(void) {
    Vector* vv = malloc(sizeof(Vector));
    vv->max = 5;
    vv->current = 0;
    vv->data = malloc(sizeof(void*) * vv->max);
    return vv;
}

void VectorDestroy(Vector* vv) {
    free(vv->data);
    free(vv);
}

void VectorResize(Vector* vv, int newSize) {
    vv->max = newSize;
    void** dd = realloc(vv->data, vv->max * sizeof(void*));
    vv->data = dd;
}

int VectorGetSize(Vector* vv) {
    return vv->current;
}

void VectorPush(Vector* vv, void* data) {
    if (vv->current >= vv->max) {
        VectorResize(vv, vv->max + 5);
    }
    vv->data[vv->current] = data;
    vv->current++;
}

void* VectorGet(Vector* vv, int index) {
    if (index < 0 || index >= vv->current) {
        printf("VectorGet: Out Of Range...\n");
    }
    return vv->data[index];
}

void VectorErase(Vector* vv, int index) {
    if (index < 0 || index >= vv->current) {
        printf("VectorErase: Out Of Range...\n");
    }
    vv->data[index] = NULL;
    for (int i = index; i < vv->current - 1; i++) {
        vv->data[i] = vv->data[i + 1];
        vv->data[i + 1] = NULL;
    }
    vv->current--;
    if (vv->current > 0 && vv->current < vv->max - 5) {
        VectorResize(vv, vv->max - 5);
    }
}

void VectorClear(Vector* vv) {
    while (VectorGetSize(vv) > 0) {
        VectorErase(vv, VectorGetSize(vv) - 1);
    }
}

bool IsColliding(SDL_Rect one, SDL_Rect two) {
    if (one.x + one.w > two.x) {
        if (two.x + two.w > one.x) {
            if (one.y + one.h > two.y) {
                if (two.y + two.h > one.y) {
                    return true;
                }
            }
        }
    }
    return false;
}

typedef struct {
    SDL_Rect bounds;
    int alpha;
    int r, g, b;
    float dir;
    float speed;
} Part;

Part* PartCreate(float x, float y) {
    Part* pp = malloc(sizeof(Part));
    pp->bounds = (SDL_Rect){x, y, 20, 20};
    pp->alpha = 255;
    pp->dir = rand() % 360;
    pp->speed = 2 + rand() % 15;
    pp->r = 0;
    pp->g = 0;
    pp->b = 0;
    int rnum = rand() % 100;
    if (rnum < 33) {
        pp->r = 255;
    } else if (rnum < 66) {
        pp->g = 255;
    } else {
        pp->b = 255;
    }
    return pp;
}

void PartDestroy(Part* pp) {
    free(pp);
}

void PartUpdate(Part* pp) {
    float xx = cos(pp->dir) * pp->speed;
    float yy = sin(pp->dir) * pp->speed;
    pp->alpha -= 2;
    pp->bounds.x += xx;
    pp->bounds.y += yy;
}

void PartRender(SDL_Renderer* rend, Part* pp) {
    SDL_SetRenderDrawColor(rend, pp->r, pp->g, pp->b, pp->alpha);
    SDL_RenderFillRect(rend, &pp->bounds);
}

typedef struct {
    SDL_Rect bounds;
    int r, g, b;
    int dir;
    float speed;
    bool dead;
} Bullet;

Bullet* BulletCreate(float x, float y, int dir) {
    Bullet* bb = malloc(sizeof(Bullet));
    bb->r = 0;
    bb->g = 0;
    bb->b = 0;
    int rnum = rand() % 100;
    if (rnum < 33) {
        bb->r = 255;
    } else if (rnum < 66) {
        bb->g = 255;
    } else {
        bb->b = 255;
    }
    bb->speed = 20.0f;
    bb->dir = dir;
    bb->bounds = (SDL_Rect){x, y, 30, 90};
    bb->dead = false;
    return bb;
}

void BulletDestroy(Bullet* bb) {
    free(bb);
}

void BulletUpdate(Bullet* bb) {
    bb->bounds.y += bb->dir * bb->speed;
    if (bb->bounds.y < -100) {
        bb->dead = true;
    }
    if (bb->bounds.y > 600) {
        bb->dead = true;
    }
}

void BulletRender(SDL_Renderer* rend, Bullet* bb) {
    SDL_SetRenderDrawColor(rend, bb->r, bb->g, bb->b, 255);
    SDL_RenderFillRect(rend, &bb->bounds);
}

typedef struct {
    SDL_Rect bounds;
    float xvel;
    float speed;
    float maxSpeed;
    int siter, stime;
    bool shooting;
} Player;

Player* PlayerCreate() {
    Player* pp = malloc(sizeof(Player));
    pp->bounds = (SDL_Rect) {475, 550, 50, 50};
    pp->xvel = 0.0f;
    pp->speed = 0.5f;
    pp->maxSpeed = 20.0f;
    pp->siter = 0;
    pp->stime = 45;
    return pp;
}

void PlayerDestroy(Player* player) {
    free(player);
}

void PlayerUpdate(Player* player, bool left, bool right, bool space) {
    if (left) {
        if (player->xvel > -player->maxSpeed) {
            player->xvel -= player->speed;
        }
    }
    if (right) {
        if (player->xvel < player->maxSpeed) {
            player->xvel += player->speed;
        }
    }
    player->bounds.x += player->xvel;
    if (player->bounds.x < 0.0f) {
        player->bounds.x = 3.0f;
        player->xvel *= -1;
    }
    if (player->bounds.x > 950.0f) {
        player->bounds.x = 947.0f;
        player->xvel *= -1;
    }
    player->siter++;
    if (space) {
        if (player->siter > player->stime) {
            player->shooting = true;
        }
    }
} 

void PlayerRender(SDL_Renderer* rend, Player* player) {
    SDL_SetRenderDrawColor(rend, 255, 0, 255, 255);
    SDL_RenderFillRect(rend, &player->bounds);
}

typedef struct {
    SDL_Rect bounds;
    float speed;
    float dspeed;
    bool isDown;
    int diter, dtime;
    int r, g, b;
    int dir;
    int siter, stime;
} En;


En* EnemyCreate(void) {
    En* en = malloc(sizeof(En));
    en->speed = 5.0f + rand() % 12;
    en->dspeed = 2.0f;
    en->bounds = (SDL_Rect){-50, 50, 50, 50};
    en->r = 0;
    en->g = 0;
    en->b = 0;
    int rnum = rand() % 100;
    if (rnum < 33) {
        en->r = 255;
    } else if (rnum < 66) {
        en->g = 255;
    } else {
        en->b = 255;
    }
    en->dir = 1;
    en->isDown = false;
    en->diter = 0;
    en->dtime = 30;
    en->siter = 0;
    en->stime = rand() % 120 + 120;
    return en;
}

void EnemyDestroy(En* en) {
    free(en);
}

void EnemyUpdate(En* en) {
    en->bounds.x += en->speed * en->dir;
    if (en->dir > 0) {
        if (en->bounds.x > 950) {
            en->bounds.x = 945;
            en->dir *= -1;
            en->isDown = false;
        }
    }
    if (en->dir < 0) {
        if (en->bounds.x < 0) {
            en->bounds.x = 5;
            en->dir *= -1;
            en->isDown = false;
        }
    }
    if (!en->isDown) {
        en->bounds.y += en->dspeed;
        en->diter++;
        if (en->diter > en->dtime) {
            en->diter = 0;
            en->isDown = true;
        }
    }
    en->siter++;
}

void EnemyRender(SDL_Renderer* rend, En* en) {
    SDL_SetRenderDrawColor(rend, en->r, en->g, en->b, 255);
    SDL_RenderFillRect(rend, &en->bounds);
}

bool left = false, right = false, space = false;

Player* player = NULL;
Vector* en = NULL;
Vector* bullets = NULL;
Vector* parts = NULL;

const int ENCAP = 50;
int spawnIter, spawnTime;

void Init() {
    player = PlayerCreate();
    bullets = VectorCreate();
    en = VectorCreate();
    parts = VectorCreate();
    VectorPush(en, EnemyCreate());
    spawnIter = 0;
    spawnTime = rand() % 120 + 30;
}

void Reset() {
    for (int i = 0; i < VectorGetSize(bullets); i++) {
        BulletDestroy((Bullet*)VectorGet(bullets, i));
    }
    VectorClear(bullets);
    for (int i = 0; i < VectorGetSize(en); i++) {
        EnemyDestroy((En*)VectorGet(en, i));
    }
    VectorClear(en);
    for (int i = 0; i < VectorGetSize(parts); i++) {
        PartDestroy((Part*)VectorGet(parts, i));
    }
    VectorClear(parts);
    player->bounds = (SDL_Rect){475, 550, 50, 50};
    player->xvel = 0.0f;
}

void Update() {
    PlayerUpdate(player, left, right, space);
    if (player->shooting) {
        player->shooting = false;
        player->siter = 0;
        VectorPush(bullets, BulletCreate(player->bounds.x, player->bounds.y - 100, -1));
    }
    for (int i = 0; i < VectorGetSize(en); i++) {
        En* ee = (En*)VectorGet(en, i);
        EnemyUpdate(ee);
        if (ee->siter > ee->stime) {
            VectorPush(bullets, BulletCreate(ee->bounds.x, ee->bounds.y + 70, 1));
            ee->siter = 0;
        }
        if (ee->bounds.y >= 550) {
            Reset();
            return;
        }
        for (int j = 0; j < VectorGetSize(bullets); j++) {
            Bullet* bb = (Bullet*)VectorGet(bullets, j);
            if (bb->dir > 0) {
                continue;
            }
            if (IsColliding(ee->bounds, bb->bounds)) {
                for (int i = 0; i < 50; i++) {
                    VectorPush(parts, PartCreate(bb->bounds.x + 10, bb->bounds.y + 50));
                }
                BulletDestroy(bb);
                VectorErase(bullets, j);
                EnemyDestroy(ee);
                VectorErase(en, i);
                break;
            }
        }
    }
    for (int i = 0; i < VectorGetSize(bullets); i++) {
        Bullet* bb = (Bullet*)VectorGet(bullets, i);
        BulletUpdate(bb);
        if (IsColliding(bb->bounds, player->bounds)) {
            Reset();
            return;
        }
        if (bb->dead) {
            BulletDestroy(bb);
            VectorErase(bullets, i);
        }
    }

    spawnIter++;
    if (spawnIter > spawnTime) {
        if (VectorGetSize(en) < ENCAP) {
            VectorPush(en, EnemyCreate());
        }
        spawnIter = 0;
        spawnTime = rand() % 120 + 30;
    }
    for (int i = 0; i < VectorGetSize(parts); i++) {
        Part* pp = (Part*)VectorGet(parts, i);
        PartUpdate(pp);
        if (pp->alpha < 5) {
            PartDestroy(pp);
            VectorErase(parts, i);
            continue;
        }
    }
}

void Render(SDL_Renderer* rend) {
    PlayerRender(rend, player);
    for (int i = 0; i < VectorGetSize(en); i++) {
        EnemyRender(rend, (En*)VectorGet(en, i));
    }
    for (int i = 0; i < VectorGetSize(bullets); i++) {
        BulletRender(rend, (Bullet*)VectorGet(bullets, i));
    }
    for (int i = 0; i < VectorGetSize(parts); i++) {
        PartRender(rend, (Part*)VectorGet(parts, i));
    }
}

void Cleanup() {
    for (int i = 0; i < VectorGetSize(bullets); i++) {
        BulletDestroy((Bullet*)VectorGet(bullets, i));
    }
    VectorClear(bullets);
    VectorDestroy(bullets);
    for (int i = 0; i < VectorGetSize(en); i++) {
        EnemyDestroy((En*)VectorGet(en, i));
    }
    VectorClear(en);
    VectorDestroy(en);
    for (int i = 0; i < VectorGetSize(parts); i++) {
        PartDestroy((Part*)VectorGet(parts, i));
    }
    VectorClear(parts);
    VectorDestroy(parts);
    PlayerDestroy(player);
}

int main(void) {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    srand(time(NULL));

    SDL_Window* window = SDL_CreateWindow("HelloFriend", 100, 100, 1000, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* rendTex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1000, 600);
    SDL_SetTextureBlendMode(rendTex, SDL_BLENDMODE_BLEND);

    const unsigned long TFPS = 1000 / 30;
    unsigned long tbegin = SDL_GetTicks();
    unsigned long ttime = SDL_GetTicks();

    bool isRunning = true;

    Init();

    while (isRunning) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                isRunning = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_w) {
                    space = true;
                }
                if (e.key.keysym.sym == SDLK_a) {
                    left = true;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    right = true;
                }
            }
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_w) {
                    space = false;
                }
                if (e.key.keysym.sym == SDLK_a) {
                    left = false;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    right = false;
                }
            }
        } 
        Update();
        SDL_SetRenderTarget(rend, rendTex);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);
        Render(rend);
        SDL_SetRenderTarget(rend, NULL);
        SDL_RenderCopy(rend, rendTex, NULL, NULL);
        SDL_RenderPresent(rend);
        tbegin = SDL_GetTicks();
        unsigned long tdelta = tbegin - ttime;
        if (tdelta < TFPS) {
            SDL_Delay(TFPS - tdelta);
        }
        ttime = SDL_GetTicks();
    }

    Cleanup();

    SDL_DestroyTexture(rendTex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
