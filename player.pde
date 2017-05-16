/*
Audio Video Player along with Photo viewer
*/

void setup()
{
size(800,600,P2D);
//fullScreen(P2D);
minim = new Minim(this);
load_images();
load_audio_file("/home/pi/Downloads/ashiqui-2/Meri-Aashiqui.mp3");
find_audio_files("/home/pi/Downloads/ashiqui-2/");
printArray(result);
}

void draw()
{
  draw_controls();
  mouse_hover();
  display_audio_info();
}