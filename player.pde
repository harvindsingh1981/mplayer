/*
Audio Video Player along with Photo viewer
*/
//PGraphics visulizer; 

void setup()
{
size(800,600,P2D);
//fullScreen(P2D);
slider_width=width-(horizontal_padding*2);//initialized Global variable because width is iniatialized after calling setup()
minim = new Minim(this);
//visulizer = createGraphics(visulizer_width,visulizer_height,P2D);
load_images();
//load_audio_file("/home/pi/Downloads/ashiqui-2/Meri-Aashiqui.mp3");
//find_audio_files("/home/pi/Downloads/ashiqui-2/");
//printArray(audio_files);
audio_profile();
}

void draw()
{
  mouse_hover();
  draw_controls();
  monitor_audio_profile();
}