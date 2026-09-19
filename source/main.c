#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_HAND 5
#define MAX_DECK 12
#define MAX_CARDS 5

typedef enum { SCREEN_MENU, SCREEN_BATTLE, SCREEN_DECK, SCREEN_WIN, SCREEN_LOSE } Screen;
typedef enum { CARD_ATTACK, CARD_BLOCK, CARD_HEAL, CARD_HEAVY } CardType;

typedef struct {
    const char *name;
    CardType type;
    int cost, value;
} Card;

typedef struct {
    const char *name;
    int hp, max_hp, intent;
    int boss;
} Enemy;

static const Card card_db[MAX_CARDS] = {
    {"Frappe", CARD_ATTACK, 1, 6},
    {"Garde", CARD_BLOCK, 1, 5},
    {"Soin", CARD_HEAL, 2, 5},
    {"Ecrasement", CARD_HEAVY, 2, 11},
    {"Petite lame", CARD_ATTACK, 0, 3}
};

static Screen screen = SCREEN_MENU;
static int cursor = 0, hp = 40, max_hp = 40, block = 0, energy = 3;
static int enemy_index = 0, battle_turn = 1;
static Enemy enemy;
static int deck[MAX_DECK], hand[MAX_HAND], hand_count = 0;
static int deck_pos = 0, victory_count = 0;

static void reset_deck(void) {
    int i;
    int base[MAX_DECK] = {0,0,0,1,1,1,2,3,4,0,1,4};
    for (i=0;i<MAX_DECK;i++) deck[i] = base[i];
    deck_pos = 0;
}

static void draw_hand(void) {
    int i;
    hand_count = 0;
    for (i=0;i<MAX_HAND;i++) {
        if (deck_pos >= MAX_DECK) deck_pos = 0;
        hand[hand_count++] = deck[deck_pos++];
    }
}

static void start_battle(int n) {
    enemy_index = n;
    if (n == 0) enemy = (Enemy){"Slime", 28, 28, 6, 0};
    else if (n == 1) enemy = (Enemy){"Garde-roche", 38, 38, 8, 0};
    else enemy = (Enemy){"Roi Cendre", 65, 65, 10, 1};

    block = 0;
    energy = 3;
    battle_turn = 1;
    draw_hand();
    screen = SCREEN_BATTLE;
}

static void enemy_turn(void) {
    int damage = enemy.intent - block;
    if (damage < 0) damage = 0;
    hp -= damage;
    block = 0;
    enemy.intent = 5 + (battle_turn % 4) + enemy.boss * 2;
    energy = 3;
    battle_turn++;
    draw_hand();
    if (hp <= 0) screen = SCREEN_LOSE;
}

static void play_card(int slot) {
    if (slot < 0 || slot >= hand_count) return;
    Card c = card_db[hand[slot]];
    if (c.cost > energy) return;

    energy -= c.cost;
    if (c.type == CARD_ATTACK) enemy.hp -= c.value;
    else if (c.type == CARD_BLOCK) block += c.value;
    else if (c.type == CARD_HEAL) {
        hp += c.value;
        if (hp > max_hp) hp = max_hp;
    } else if (c.type == CARD_HEAVY) enemy.hp -= c.value;

    hand[slot] = -1;
    if (enemy.hp <= 0) {
        victory_count++;
        if (enemy.boss) screen = SCREEN_WIN;
        else start_battle(enemy_index + 1);
    }
}

static void draw_menu(void) {
    printf("\x1b[1;1H=== ASHEN DECK ===");
    printf("\x1b[3;1HUn roguelike de cartes original");
    printf("\x1b[6;1H[A] Commencer une partie");
    printf("\x1b[7;1H[B] Voir le deck");
    printf("\x1b[9;1HSTART Quitter");
}

static void draw_deck(void) {
    int i;
    printf("\x1b[1;1H=== MON DECK ===");
    for (i=0;i<MAX_DECK;i++) {
        printf("\x1b[%d;1H%d. %s", 3+i, i+1, card_db[deck[i]].name);
    }
    printf("\x1b[16;1HB/START : retour");
}

static void draw_battle(void) {
    int i;
    printf("\x1b[1;1H=== COMBAT %d ===", battle_turn);
    printf("\x1b[3;1H%s %d/%d", enemy.name, enemy.hp, enemy.max_hp);
    printf("\x1b[4;1HIntention : %d degats", enemy.intent);
    printf("\x1b[6;1HVous : %d/%d  Garde:%d  Energie:%d", hp,max_hp,block,energy);
    printf("\x1b[8;1HCartes :");
    for (i=0;i<hand_count;i++) {
        if (hand[i] >= 0)
            printf("\x1b[%d;1H[%d] %s (%dE)", 9+i, i+1,
                   card_db[hand[i]].name, card_db[hand[i]].cost);
    }
    printf("\x1b[15;1HA-Dpad : choisir carte");
    printf("\x1b[16;1HA : jouer  X : fin du tour");
}

int main(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    reset_deck();
    srand((unsigned)time(NULL));

    while (aptMainLoop()) {
        hidScanInput();
        u32 k = hidKeysDown();

        if (k & KEY_START) {
            if (screen == SCREEN_MENU) break;
            screen = SCREEN_MENU;
        }

        if (screen == SCREEN_MENU) {
            if (k & KEY_A) {
                hp = max_hp;
                reset_deck();
                start_battle(0);
            } else if (k & KEY_B) {
                screen = SCREEN_DECK;
            }
        } else if (screen == SCREEN_DECK) {
            if (k & KEY_B) screen = SCREEN_MENU;
        } else if (screen == SCREEN_BATTLE) {
            if (k & KEY_LEFT) cursor--;
            if (k & KEY_RIGHT) cursor++;
            if (cursor < 0) cursor = 0;
            if (cursor >= hand_count) cursor = hand_count - 1;

            if (k & KEY_A) play_card(cursor);
            if (k & KEY_X) enemy_turn();
        } else if (screen == SCREEN_WIN || screen == SCREEN_LOSE) {
            if (k & KEY_A) {
                screen = SCREEN_MENU;
                reset_deck();
            }
        }

        consoleClear();
        if (screen == SCREEN_MENU) draw_menu();
        else if (screen == SCREEN_DECK) draw_deck();
        else if (screen == SCREEN_BATTLE) draw_battle();
        else if (screen == SCREEN_WIN) {
            printf("\x1b[5;1H=== VICTOIRE ! ===");
            printf("\x1b[7;1HLe Roi Cendre est vaincu.");
            printf("\x1b[9;1HA : retour au menu");
        } else {
            printf("\x1b[5;1H=== DEFAITE ===");
            printf("\x1b[7;1HVotre aventure s'acheve.");
            printf("\x1b[9;1HA : retour au menu");
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
