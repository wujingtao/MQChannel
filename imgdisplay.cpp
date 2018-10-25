#include "imgdisplay.h"
#include "qdebug.h"

imgDisplay::imgDisplay()
{
    video.initWindow();
}
void imgDisplay::run(){
    while(1)
    {

        video.readImg();
        if(exitapp)
        {
             return ;
        }
        delaymsec(1000);
    }
}
void imgDisplay::exit(){
       video.closeImg();
}
