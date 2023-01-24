
#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
 
#define OT_LENGTH 1 // the ordertable length
#define PACKETMAX 500 // the maximum number of objects on the screen (please note that i added more than on the hello word example so it can display all the triangles correctly)
#define SCREEN_WIDTH  320 // screen width-
#define SCREEN_HEIGHT 240 // screen height (240 NTSC, 256 PAL)
 
GsOT myOT[2]; // ordering table header
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH]; // ordering table unit
PACKET GPUPacketArea[2][PACKETMAX]; // GPU packet data
 
u_long _ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long _stacksize = 0x00004000; // force 16 kilobytes of stack
 

short CurrentBuffer = 0; // holds the current buffer number
 
long Counter = 0;
short RGBColorFlag[3] = {0, 0, 0};
short RGBColor[3] = {0, 0, 0};
 

void graphics(); // inits the GPU
void display(); // updates the GPU (IE: VRAM/SGRAM/framebuffer)
 
const DEBUG = 1; // debugging (1=on, 0=off)
 

POLY_F3 pf3;
 
void DrawSolidTriangle(short x1, short y1, short x2, short y2, short x3, short y3, int r, int g, int b, int ot)
{
    // draw a solid triangle and fill it with rgb colour
    // EG: DrawSolidTri(200,0, 170,30, 230,30, 255,0,255, 0);
    // check the Psy-Q documentation for the coordinates!
 
    SetPolyF3(&pf3);                            // initialise the primitive
    setRGB0(&pf3, r, g, b);                     // set the fill colour (RGB)
    setXY3(&pf3, x1,y1, x2,y2, x3,y3);          // draw 3 lines (3 sets of x/y co-ords)
    DrawPrim(&pf3);                             // copy shape to frame buffer
    GsSortPoly(&pf3, &myOT[CurrentBuffer], ot); // put the polygon into the order table
}
 
 
void DrawTransTriangle(short x1, short y1, short x2, short y2, short x3, short y3, int r, int g, int b, int ot)
{
    // draw a transparent triangle
    // the transparency 'colour' is calculated from the fgnd & bgns pixels.
    // EG: DrawTransTri(50,0, 70,60, 130,60, 255,0,255, 0);
 
    SetPolyF3(&pf3);                            // initialize primitive
    setRGB0(&pf3, r, g, b);                     // set the foreground fill colour
    setSemiTrans(&pf3, 1);                      // semi-transparency; 0=off; 1=on
    setXY3(&pf3, x1,y1, x2,y2, x3,y3);          // draw 4 lines (4 sets of x/y co-ords)
    DrawPrim(&pf3);                         // copy shape to frame buffer
    GsSortPoly(&pf3, &myOT[CurrentBuffer], ot); // put the polygon into the order table
}
 
// ---------
// COLOR CHANGES
// ---------
int ColorAnimation(){
    if (RGBColorFlag[0] == 0){
        RGBColor[0] += 2;
        if (RGBColor[0] >= 252){ // When the Red color reaches high enough, this changes to 1, and the color goes backwards
            RGBColorFlag[0] = 1;
            }
        }
    else {  RGBColor[0] -= 2;
            if (RGBColor[0] <= 3){
            RGBColorFlag[0] = 0;
            }
        }
    if (RGBColorFlag[1] == 0){
        RGBColor[1] += 1;
        if (RGBColor[1] >= 252){ // When the Green color reaches high enough, this changes to 1, and the color goes backwards
            RGBColorFlag[1] = 1;
            }
        }
    else {  RGBColor[1] -= 1;
            if (RGBColor[1] <= 3){
            RGBColorFlag[1] = 0;
            }
        }
    if (RGBColorFlag[2] == 0){
        RGBColor[2] += 3;
        if (RGBColor[2] >= 252){ // When the Blue color reaches high enough, this changes to 1, and the color goes backwards
            RGBColorFlag[2] = 1;
            }
        }
    else {  RGBColor[2] -= 3;
            if (RGBColor[2] <= 3){
            RGBColorFlag[2] = 0;
            }
        }
}
 

int main()
{
    graphics(); // setup the graphics (seen below)
    FntLoad(960, 256); // load the font from the BIOS into the framebuffer
    SetDumpFnt(FntOpen(5, 20, 320, 240, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters
   
    if (DEBUG) // should debug be true (equal 1)
    {
        // print to the TTY stream (only visible if using one)
        printf("\n\nHello World\n");
        printf("\nhttp://psxdev.net/");
    }
 
    while (1) // draw and display forever
    {
        DrawSolidTriangle(40, 60, 160, 180, 40, 180, 0, RGBColor[1], 0, 0); //Green Solid Triangle at the lower left
        DrawSolidTriangle(40, 60, 160, 60, 160, 180, RGBColor[0], RGBColor[1], RGBColor[2], 1); //Multi Colored Solid Triangle at the upper left
        DrawTransTriangle(160, 60, 280, 180, 160, 180, 0, RGBColor[1], 0, 2); //Green Transparent Triangle at the lower right
        DrawTransTriangle(160, 60, 280, 60, 280, 180, RGBColor[0], RGBColor[1], RGBColor[2], 3); //Multi Colores Transparent Triangle at the upper right
        ColorAnimation(); // Actively changes the RGBColor array values
        FntPrint("             HELLO WORLD\n\n          HTTP://PSXDEV.NET/");
        FntPrint("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // Making spaces so put the next texts in place
        FntPrint("\n Counter is = %d\r", Counter);
        FntPrint("\n RGBColorFlag[0] is = %d\r", RGBColorFlag[0]); // Displays the value of the Red flag
        FntPrint("\n RGBColorFlag[1] is = %d\r", RGBColorFlag[1]); // Displays the value of the Green flag
        FntPrint("\n RGBColorFlag[2] is = %d\r", RGBColorFlag[2]); // Displays the value of the Blue flag
        FntPrint("\n RGBColor[0] is = %d\r", RGBColor[0]); // This display the current Red color value
        FntPrint("\n RGBColor[1] is = %d\r", RGBColor[1]); // This display the current Green color value
        FntPrint("\n RGBColor[2] is = %d\r", RGBColor[2]); // This display the current Blue color value
        display();
        Counter++;
    }
 
    return 0; // this will never be reached because we're in a while loop above
}
 
void graphics()
{
    SetVideoMode(1); // PAL mode
    //SetVideoMode(0); // NTSC mode
   
    GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0); // set the graphics mode resolutions (GsNONINTER for NTSC, and GsINTER for PAL)
    GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT); // tell the GPU to draw from the top left coordinates of the framebuffer
   
    // init the ordertables
    myOT[0].length = OT_LENGTH;
    myOT[1].length = OT_LENGTH;
    myOT[0].org = myOT_TAG[0];
    myOT[1].org = myOT_TAG[1];
   
    // clear the ordertables
    GsClearOt(0,0,&myOT[0]);
    GsClearOt(0,0,&myOT[1]);
}
 
 
void display()
{
    // refresh the font
    FntFlush(-1);
   
    // get the current buffer
    CurrentBuffer = GsGetActiveBuff();
   
    // setup the packet workbase
    GsSetWorkBase((PACKET*)GPUPacketArea[CurrentBuffer]);
   
    // clear the ordering table
    GsClearOt(0,0, &myOT[CurrentBuffer]);
   
    // wait for all drawing to finish
    DrawSync(0);
   
    // wait for v_blank interrupt
    VSync(0);
   
    // flip the double buffers
    GsSwapDispBuff();
   
    // clear the ordering table with a background color (R,G,B)
    GsSortClear(50,50,50, &myOT[CurrentBuffer]);
   
    // draw the ordering table
    GsDrawOt(&myOT[CurrentBuffer]);
}