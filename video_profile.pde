import processing.awt.PSurfaceAWT;
import processing.awt.PSurfaceAWT.SmoothCanvas;
int get_screen_x()
{
  return ((SmoothCanvas)((PSurfaceAWT)surface).getNative()).getFrame().getX();
}

int get_screen_y()
{
  return ((SmoothCanvas)((PSurfaceAWT)surface).getNative()).getFrame().getY();
}

void draw_controls_video()
{
image(img_background,0,0,width,height);
image(img_home,home_x,home_y,home_width,home_height);
}

void video_profile()
{
draw_controls_video();  
}

void monitor_video_profile()
{
draw_controls_video();  
}