int img_width=300;
int img_height=300;
int img_sound_x;
int img_sound_y;
int img_video_x;
int img_video_y;

void draw_controls_home()
{
int spacing;
int img_width=300;
int img_height=300;
  spacing=((width-(img_width*2))/3);
  img_sound_x=spacing;
  img_sound_y=(height/2)-(img_height/2);
  img_video_x=(spacing*2)+img_width;
  img_video_y=(height/2)-(img_height/2);

  image(img_background,0,0,width,height);
  image(img_sound,img_sound_x,img_sound_y,img_width,img_height);
  image(img_video,img_video_x,img_video_y,img_width,img_height);
}

void home_profile()
{
draw_controls_home();  
}

void monitor_home_profile()
{
draw_controls_home();  
}