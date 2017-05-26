/*
Audio Video Player along with Photo viewer
*/
//PGraphics visulizer; 
String current_profile="";
int profile_switch=0;

void setup()
{
size(800,600,P3D);
//fullScreen(P2D);
slider_width=width-(horizontal_padding*2);//initialized Global variable because width is iniatialized after calling setup()
minim = new Minim(this);
//visulizer = createGraphics(visulizer_width,visulizer_height,P2D);
load_images();
//load_audio_file("/home/pi/Downloads/ashiqui-2/Meri-Aashiqui.mp3");
//find_audio_files("/home/pi/Downloads/ashiqui-2/");
//printArray(audio_files);
//audio_profile();
run_js();
}

void draw()
{
  switch(current_profile)
  {
    case "":
    current_profile="HOME";
    home_profile();
    break;
    
    case "HOME":
      if(profile_switch==1)
        {
          home_profile();
          profile_switch=0;
        }
       monitor_home_profile();   
    break;
    
    case "AUDIO":
      if(profile_switch==1)
        {
          audio_profile();
          profile_switch=0;
        }
      draw_controls_audio();
      monitor_audio_profile();
    break;
    
    case "VIDEO":
      if(profile_switch==1)
        {
          video_profile();
          profile_switch=0;
        }
       monitor_video_profile();   
    break;
    
    case "IMAGE":
    break;
  }
}