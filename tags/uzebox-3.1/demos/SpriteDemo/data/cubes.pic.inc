//picture: cubes.raw
//Horizontal Tiles=29
//Vertical Tiles=2
//Tile Width=6
//Tile Height=8
#define CUBES_TILESET_SIZE 58

const char cubes_tileset[] PROGMEM ={
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00		 //tile:0 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x0d, 0x0d, 0xfe, 0xfe, 0xfe, 0x0c, 0x0f, 0x0e, 0xfe, 0xfe, 0xfe, 0x0c, 0x0e, 0x0e, 0xfe, 0xfe, 0xfe, 0x0d, 0x0e, 0x0e, 0xfe, 0xfe, 0x00, 0x0e, 0x0e, 0x0e, 0xfe, 0xfe, 0x01, 0x0e, 0x0e, 0x0e, 0xfe, 0xfe, 0x01, 0x0e, 0x0e, 0x0e		 //tile:1 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x02, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x03, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x0c, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x0c, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x0d, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0d, 0x00, 0x0e, 0x0d, 0x0d, 0x0d, 0x0d, 0x01		 //tile:2 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x0d, 0x0d, 0x0c, 0xfe, 0xfe, 0x03, 0x0e, 0x0d, 0x0d, 0xfe, 0xfe, 0x03, 0x0e, 0x0d, 0x0d, 0xfe, 0xfe, 0x0c, 0x0e, 0x0d, 0x0d, 0xfe, 0xfe, 0x0c, 0x0e, 0x0d, 0x0d, 0xfe, 0xfe, 0x0c, 0x0e, 0x0d, 0x0d, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d		 //tile:3 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0c, 0x0c, 0x0c, 0x01, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x03, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x0c, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x01, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x02, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x03, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x0c, 0xfe		 //tile:4 
, 0xfe, 0xfe, 0xfe, 0x01, 0x00, 0xfe, 0xfe, 0xfe, 0x03, 0x0d, 0x0c, 0x0c, 0xfe, 0xfe, 0x0c, 0x0d, 0x0c, 0x0c, 0xfe, 0xfe, 0x0d, 0x0d, 0x0c, 0x0c, 0xfe, 0xfe, 0x0e, 0x0e, 0x0c, 0x0c, 0xfe, 0x01, 0x0f, 0x0e, 0x0c, 0x0c, 0xfe, 0x02, 0x0f, 0x0e, 0x0c, 0x0c, 0xfe, 0x03, 0x0f, 0x0f, 0x0d, 0x0c		 //tile:5 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x03, 0x02, 0xfe, 0xfe, 0xfe, 0x0c, 0x0c, 0x0c, 0x02, 0xfe, 0xfe, 0x0c, 0x0c, 0x0c, 0x03, 0xfe, 0xfe, 0x0c, 0x0c, 0x0c, 0x0c, 0x01, 0xfe, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0xfe, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x02		 //tile:6 
, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x03, 0x0d, 0x03, 0x03, 0xfe, 0xfe, 0x0d, 0x0e, 0x03, 0x03, 0xfe, 0xfe, 0x0e, 0x0f, 0x0c, 0x03, 0xfe, 0x01, 0x0f, 0x0f, 0x0d, 0x03, 0xfe, 0x03, 0x0f, 0x0f, 0x0e, 0x03, 0xfe, 0x0d, 0x0f, 0x0f, 0x0f, 0x0c, 0x01, 0x0e, 0x0f, 0x0f, 0x0f, 0x0d		 //tile:7 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x02, 0x01, 0xfe, 0xfe, 0xfe, 0x0b, 0x03, 0x03, 0xfe, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x02, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x03, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x02, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02		 //tile:8 
, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x4e, 0x03, 0xfe, 0xfe, 0xfe, 0x0c, 0x57, 0x0c, 0xfe, 0xfe, 0xfe, 0x0e, 0x57, 0x4e, 0xfe, 0xfe, 0x02, 0x4f, 0x4f, 0x57, 0xfe, 0xfe, 0x0d, 0x4f, 0x4f, 0x57, 0xfe, 0x01, 0x0f, 0x4f, 0x4f, 0x4f, 0xfe, 0x0b, 0x4f, 0x4f, 0x4f, 0x4f		 //tile:9 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0xfe, 0x02, 0x03, 0x03, 0x02, 0xfe, 0xfe, 0x03, 0x02, 0x02, 0x03, 0x01, 0xfe, 0x0c, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x4e, 0x03, 0x02, 0x02, 0x03, 0x01, 0x57, 0x0d, 0x02, 0x02, 0x02, 0x02, 0x4f, 0x4f, 0x03, 0x02, 0x02, 0x02		 //tile:10 
, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0x55, 0x4d, 0xfe, 0xfe, 0xfe, 0x02, 0x57, 0x57, 0xfe, 0xfe, 0xfe, 0x4d, 0x57, 0x57, 0xfe, 0xfe, 0x02, 0x57, 0x57, 0x57, 0xfe, 0xfe, 0x0d, 0x57, 0x57, 0x57, 0x00, 0x02, 0x57, 0x57, 0x57, 0x57, 0x02, 0x0d, 0x4f, 0x4f, 0x57, 0x57		 //tile:11 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0x0b, 0x01, 0x02, 0xfe, 0xfe, 0xfe, 0x56, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0x57, 0x0c, 0x01, 0x02, 0x01, 0xfe, 0x57, 0x57, 0x02, 0x01, 0x02, 0x00, 0x57, 0x57, 0x4d, 0x01, 0x02, 0x01, 0x57, 0x57, 0x57, 0x0b, 0x01, 0x02		 //tile:12 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0b, 0xfe, 0xfe, 0xfe, 0xfe, 0x0a, 0x57, 0xfe, 0xfe, 0xfe, 0xfe, 0x56, 0x57, 0xfe, 0xfe, 0xfe, 0x0b, 0x57, 0x57, 0xfe, 0xfe, 0x01, 0x56, 0x57, 0x57, 0xfe, 0xfe, 0x0c, 0x57, 0x57, 0x57, 0xfe, 0x02, 0x57, 0x57, 0x57, 0x4e, 0x01, 0x4d, 0x57, 0x4e, 0x4e, 0x4e		 //tile:13 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0c, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x57, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x57, 0x4d, 0x01, 0xfe, 0xfe, 0xfe, 0x57, 0x57, 0x0c, 0x01, 0xfe, 0xfe, 0x56, 0x4e, 0x4e, 0x02, 0x01, 0xfe, 0x4e, 0x4e, 0x4e, 0x0d, 0x01, 0x01, 0x4e, 0x4e, 0x4e, 0x4e, 0x0c, 0x01		 //tile:14 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0c, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x4e, 0xfe, 0xfe, 0xfe, 0x00, 0x0d, 0x4e, 0xfe, 0xfe, 0xfe, 0x0c, 0x4e, 0x0d, 0xfe, 0xfe, 0x02, 0x4e, 0x0e, 0x0d, 0xfe, 0x00, 0x0d, 0x4e, 0x0d, 0x0d, 0x00, 0x0c, 0x4e, 0x0d, 0x0d, 0x0d		 //tile:15 
, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x4e, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0x4e, 0x0d, 0x00, 0xfe, 0xfe, 0xfe, 0x0d, 0x0e, 0x0b, 0xfe, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x02, 0xfe, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x01, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x0c, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x02		 //tile:16 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x03, 0xfe, 0xfe, 0xfe, 0x02, 0x03, 0x03, 0xfe, 0xfe, 0x01, 0x03, 0x03, 0x03, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x03, 0xfe, 0x02, 0x03, 0x03, 0x03, 0x03		 //tile:17 
, 0x01, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x0b, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x03, 0x01, 0xfe, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x00, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x02, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x01, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x03, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02		 //tile:18 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0c, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x0f, 0xfe, 0xfe, 0xfe, 0x03, 0x4f, 0x03, 0xfe, 0xfe, 0x01, 0x0f, 0x0d, 0x02, 0xfe, 0xfe, 0x0d, 0x0e, 0x02, 0x02, 0xfe, 0x0b, 0x4f, 0x03, 0x02, 0x02, 0x02, 0x0f, 0x0c, 0x02, 0x02, 0x02		 //tile:19 
, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x01, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01		 //tile:20 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x0d, 0xfe, 0xfe, 0xfe, 0x02, 0x0f, 0x4f, 0xfe, 0xfe, 0xfe, 0x0e, 0x4f, 0x0f, 0xfe, 0xfe, 0x0c, 0x4f, 0x4f, 0x0c, 0xfe, 0x02, 0x4f, 0x4f, 0x0e, 0x02, 0x00, 0x0e, 0x4f, 0x4f, 0x03, 0x01, 0x0c, 0x4f, 0x4f, 0x0d, 0x01, 0x01		 //tile:21 
, 0x01, 0x03, 0x00, 0xfe, 0xfe, 0xfe, 0x0f, 0x0c, 0x01, 0xfe, 0xfe, 0xfe, 0x0e, 0x01, 0x01, 0x00, 0xfe, 0xfe, 0x02, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x00, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01		 //tile:22 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x0c, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x4f, 0xfe, 0xfe, 0xfe, 0x03, 0x4f, 0x0f, 0xfe, 0xfe, 0xfe, 0x0e, 0x4f, 0x0f, 0xfe, 0xfe, 0x0c, 0x4f, 0x4f, 0x4f, 0xfe, 0x02, 0x4f, 0x4f, 0x4f, 0x4f, 0xfe, 0x0d, 0x4f, 0x4f, 0x4f, 0x0e		 //tile:23 
, 0xfe, 0x01, 0x02, 0x02, 0xfe, 0xfe, 0x0d, 0x0e, 0x0f, 0x03, 0xfe, 0xfe, 0x0f, 0x4f, 0x0d, 0x01, 0x01, 0xfe, 0x0f, 0x0f, 0x02, 0x01, 0x01, 0xfe, 0x4f, 0x0c, 0x01, 0x01, 0x01, 0x00, 0x0f, 0x02, 0x01, 0x01, 0x01, 0x01, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01		 //tile:24 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03, 0x0c, 0xfe, 0xfe, 0xfe, 0x01, 0x0f, 0x4f, 0xfe, 0xfe, 0xfe, 0x0d, 0x4f, 0x0f, 0xfe, 0xfe, 0x02, 0x4f, 0x4f, 0x4f, 0xfe, 0xfe, 0x0d, 0x4f, 0x4f, 0x4f, 0x00, 0x02, 0x4f, 0x4f, 0x4f, 0x4f, 0x01, 0x0d, 0x4f, 0x4f, 0x4f, 0x4f		 //tile:25 
, 0xfe, 0x00, 0x01, 0x02, 0x00, 0xfe, 0x0d, 0x0e, 0x0f, 0x0e, 0x00, 0xfe, 0x4f, 0x0f, 0x4f, 0x0b, 0x01, 0xfe, 0x4f, 0x4f, 0x0e, 0x02, 0x01, 0xfe, 0x4f, 0x4f, 0x0d, 0x01, 0x01, 0x00, 0x4f, 0x4f, 0x02, 0x01, 0x01, 0x01, 0x4f, 0x0e, 0x01, 0x01, 0x01, 0x01, 0x4f, 0x0c, 0x01, 0x01, 0x01, 0x01		 //tile:26 
, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x0d, 0x0d, 0xfe, 0xfe, 0xfe, 0x0c, 0x4f, 0x0f, 0xfe, 0xfe, 0x00, 0x0e, 0x0f, 0x0f, 0xfe, 0xfe, 0x01, 0x4f, 0x4f, 0x0f, 0xfe, 0xfe, 0x0b, 0x4f, 0x4f, 0x4f, 0xfe, 0xfe, 0x0d, 0x4f, 0x4f, 0x4f, 0xfe, 0x00, 0x4e, 0x4f, 0x4f, 0x4f		 //tile:27 
, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x0d, 0x0d, 0x0e, 0x0e, 0x02, 0xfe, 0x0f, 0x0e, 0x0e, 0x0e, 0x02, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x02, 0xfe, 0x0f, 0x0e, 0x0e, 0x0e, 0x02, 0xfe, 0x0f, 0x0e, 0x0e, 0x0e, 0x02, 0xfe, 0x4f, 0x0f, 0x0e, 0x0e, 0x02, 0xfe, 0x4e, 0x0e, 0x0e, 0x0e, 0x02, 0xfe		 //tile:28 
, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00		 //tile:29 
, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:30 
, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:31 
, 0xfe, 0xfe, 0x0c, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x0c, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0x03, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0x03, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0x02, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0x02, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:32 
, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x02, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:33 
, 0xfe, 0x03, 0x0f, 0x0f, 0x03, 0x01, 0xfe, 0x02, 0x0f, 0x0e, 0x02, 0x01, 0xfe, 0x01, 0x0f, 0x0d, 0x01, 0x01, 0xfe, 0xfe, 0x0e, 0x0c, 0x01, 0x01, 0xfe, 0xfe, 0x0d, 0x03, 0x01, 0x01, 0xfe, 0xfe, 0x0c, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0x02, 0x02, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:34 
, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x00, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x02, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:35 
, 0x01, 0x0e, 0x0f, 0x0f, 0x0f, 0x0d, 0xfe, 0x0d, 0x0f, 0x0f, 0x0f, 0x03, 0xfe, 0x03, 0x0f, 0x0f, 0x0e, 0x01, 0xfe, 0x02, 0x0f, 0x0f, 0x0c, 0x01, 0xfe, 0xfe, 0x0e, 0x0e, 0x02, 0x01, 0xfe, 0xfe, 0x0c, 0x0d, 0x01, 0x01, 0xfe, 0xfe, 0x03, 0x03, 0x01, 0x01, 0xfe, 0xfe, 0x00, 0x01, 0x00, 0xfe		 //tile:36 
, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x00, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:37 
, 0xfe, 0x0c, 0x0f, 0x0f, 0x4f, 0x4f, 0xfe, 0x01, 0x0f, 0x0f, 0x0f, 0x4f, 0xfe, 0xfe, 0x0d, 0x0f, 0x0f, 0x4f, 0xfe, 0xfe, 0x02, 0x0f, 0x0e, 0x4f, 0xfe, 0xfe, 0xfe, 0x0e, 0x0f, 0x0e, 0xfe, 0xfe, 0xfe, 0x0b, 0x0f, 0x0c, 0xfe, 0xfe, 0xfe, 0x01, 0x0e, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x00		 //tile:38 
, 0x4f, 0x4f, 0x02, 0x01, 0x01, 0x02, 0x4f, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0e, 0x02, 0x01, 0x01, 0x01, 0x01, 0x0c, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x02, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x02, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:39 
, 0x01, 0x0e, 0x4f, 0x4f, 0x4f, 0x4f, 0xfe, 0x02, 0x4f, 0x4f, 0x4f, 0x4f, 0xfe, 0xfe, 0x0d, 0x4f, 0x4e, 0x4e, 0xfe, 0xfe, 0x02, 0x4f, 0x4e, 0x4e, 0xfe, 0xfe, 0xfe, 0x0d, 0x4f, 0x4e, 0xfe, 0xfe, 0xfe, 0x02, 0x0e, 0x0e, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x0d, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x01		 //tile:40 
, 0x4f, 0x57, 0x57, 0x0b, 0x01, 0x01, 0x4f, 0x4f, 0x0d, 0x01, 0x01, 0x01, 0x4f, 0x4e, 0x02, 0x01, 0x01, 0x00, 0x4f, 0x0c, 0x01, 0x01, 0x01, 0xfe, 0x0d, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x03, 0x01, 0x01, 0x00, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:41 
, 0x01, 0x4e, 0x4f, 0x4e, 0x4e, 0x4e, 0x00, 0x02, 0x4f, 0x4e, 0x4e, 0x4e, 0xfe, 0xfe, 0x0c, 0x4e, 0x4e, 0x4e, 0xfe, 0xfe, 0xfe, 0x0e, 0x0e, 0x0e, 0xfe, 0xfe, 0xfe, 0x0b, 0x0e, 0x0e, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x0e, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x0e, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x03		 //tile:42 
, 0x4e, 0x4e, 0x4e, 0x4e, 0x0c, 0x01, 0x4e, 0x4e, 0x4e, 0x0d, 0x02, 0x01, 0x0e, 0x0e, 0x0e, 0x02, 0x01, 0xfe, 0x0e, 0x0e, 0x0c, 0x01, 0x00, 0xfe, 0x0e, 0x0d, 0x01, 0x01, 0xfe, 0xfe, 0x0d, 0x02, 0x01, 0xfe, 0xfe, 0xfe, 0x0b, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:43 
, 0x00, 0x0c, 0x0e, 0x0d, 0x0d, 0x0d, 0xfe, 0x01, 0x0d, 0x0d, 0x0d, 0x0d, 0xfe, 0xfe, 0x02, 0x0d, 0x0d, 0x0d, 0xfe, 0xfe, 0xfe, 0x0c, 0x0d, 0x0d, 0xfe, 0xfe, 0xfe, 0xfe, 0x0d, 0x0d, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x0d, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x0b, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x01		 //tile:44 
, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x03, 0x0d, 0x0d, 0x0d, 0x0d, 0x0c, 0xfe, 0x0d, 0x0d, 0x0d, 0x0d, 0x01, 0xfe, 0x0d, 0x0d, 0x0d, 0x02, 0xfe, 0xfe, 0x0d, 0x0d, 0x0b, 0xfe, 0xfe, 0xfe, 0x0d, 0x0c, 0x01, 0xfe, 0xfe, 0xfe, 0x0d, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:45 
, 0xfe, 0x02, 0x03, 0x03, 0x03, 0x03, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x03, 0xfe, 0xfe, 0x01, 0x03, 0x03, 0x03, 0xfe, 0xfe, 0xfe, 0x02, 0x03, 0x03, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x03, 0xfe, 0xfe, 0xfe, 0xfe, 0x00, 0x03, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:46 
, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x01, 0xfe, 0x03, 0x03, 0x03, 0x02, 0xfe, 0xfe, 0x03, 0x03, 0x03, 0x00, 0xfe, 0xfe, 0x03, 0x03, 0x01, 0xfe, 0xfe, 0xfe, 0x03, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x00, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:47 
, 0x01, 0x03, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x01, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x00, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:48 
, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x00, 0xfe, 0x02, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:49 
, 0x02, 0x03, 0x03, 0x02, 0x01, 0x01, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0xfe, 0x01, 0x02, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x00, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:50 
, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x00, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x02, 0x01, 0x01, 0x00, 0xfe, 0xfe, 0x02, 0x02, 0x01, 0xfe, 0xfe, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0xfe, 0xfe		 //tile:51 
, 0xfe, 0x03, 0x03, 0x03, 0x03, 0x02, 0xfe, 0x00, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x00, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:52 
, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, 0x02, 0x02, 0x01, 0x01, 0x01, 0xfe, 0x02, 0x02, 0x02, 0x01, 0x01, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x00, 0xfe, 0xfe, 0x00, 0x01, 0x01, 0xfe, 0xfe		 //tile:53 
, 0x01, 0x02, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x00, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:54 
, 0x03, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x01, 0x01, 0x00, 0x02, 0x02, 0x02, 0x01, 0x01, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x01, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x00, 0xfe, 0xfe, 0xfe, 0x00, 0x01, 0xfe, 0xfe		 //tile:55 
, 0xfe, 0x00, 0x02, 0x02, 0x02, 0x02, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x00, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0x01, 0x02, 0x01, 0xfe, 0xfe, 0xfe, 0x00, 0x01, 0x01, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe		 //tile:56 
, 0x02, 0x02, 0x02, 0x02, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xfe		 //tile:57 
};