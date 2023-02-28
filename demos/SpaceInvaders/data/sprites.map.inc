
#define mapTransparent_Width   (1)
#define mapTransparent_Height  (1)

const char mapTransparent[1 * 1 + 2] PROGMEM=
{mapTransparent_Width,mapTransparent_Height,
    0
};

#define mapUfo_Width   (2)
#define mapUfo_Height  (1)

const char mapUfo[2 * 1 + 2] PROGMEM=
{mapUfo_Width,mapUfo_Height,
    1,  2
};

#define animShip_Width   (2)
#define animShip_Height  (1)

const char animShip[2 * 1] PROGMEM=
{
    3,  4
};

#define animShipDestroyed_Width   (2)
#define animShipDestroyed_Height  (2)

const char animShipDestroyed[2 * 2] PROGMEM=
{
    5,  6,
    7,  8
};

#define animPlayerShoot_Width   (1)
#define animPlayerShoot_Height  (1)

const char animPlayerShoot[1 * 1] PROGMEM=
{
    9
};

#define animInvaderShoot_Width   (1)
#define animInvaderShoot_Height  (3)

const char animInvaderShoot[1 * 3] PROGMEM=
{
   10,
   11,
   12
};
