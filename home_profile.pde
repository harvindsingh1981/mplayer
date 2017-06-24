int img_width=300;
int img_height=300;
int img_sound_x;
int img_sound_y;
int img_video_x;
int img_video_y;
int img_video_recording_x;
int img_video_recording_y;
//void draw_controls_home()
//{
//int spacing;
//int img_width=300;
//int img_height=300;
//  spacing=((width-(img_width*2))/3);
//  img_sound_x=spacing;
//  img_sound_y=(height/2)-(img_height/2);
//  img_video_x=(spacing*2)+img_width;
//  img_video_y=(height/2)-(img_height/2);

//  image(img_background,0,0,width,height);
//  image(img_sound,img_sound_x,img_sound_y,img_width,img_height);
//  image(img_video,img_video_x,img_video_y,img_width,img_height);
//}
int[]draw_icon(int position,PImage img,int total_buttons,int total_columns)
{
  int[] x_y_arr={0,0,0,0};
  int row,column,total_rows,icon_width,icon_height,image_width,image_height,icon_x,icon_y,image_x,image_y;
  int h_padding=20;
  int v_padding=20;
  int margin_width=20;
  if(total_buttons%total_columns==0)total_rows=total_buttons/total_columns;
  else total_rows=(total_buttons/total_columns)+1;
  row=(position-1)/total_columns;
  column=(position-1)%total_columns;
  icon_width=(width-((h_padding*2)+((total_columns-1)*margin_width)))/total_columns;
  icon_height=(height-((v_padding*2)+((total_rows-1)*margin_width)))/total_rows;
  icon_x=h_padding+(column*margin_width)+(column*icon_width);
  icon_y=v_padding+(row*margin_width)+(row*icon_height);
  /***********below code is for displaying square image over rectangular icon*********/
  if(icon_width!=icon_height)
  {
    if(icon_width<icon_height)
    {
      image_width=icon_width;
      image_height=icon_width;
      image_x=icon_x;
      image_y=icon_y+(icon_height/2)-(image_height/2);
    }
    else
    {
      image_width=icon_height;
      image_height=icon_height;
      image_x=icon_x+(icon_width/2)-(image_width/2);
      image_y=icon_y;
    }
  }
  else
  {
    image_x=icon_x;
    image_y=icon_y;
    image_width=icon_width;
    image_height=icon_height;
  }
  /************************************************************************************/
  x_y_arr[0]=image_x;
  x_y_arr[1]=image_y;
  x_y_arr[2]=image_width;
  x_y_arr[3]=image_height;
  //rect(icon_x,icon_y,icon_width,icon_height);
  image(img,image_x,image_y,image_width,image_height);
  return x_y_arr;
}
void draw_controls_home()
{
int total_buttons=3;
int total_columns=2;
int[]arr={};
  image(img_background,0,0,width,height);
  arr=draw_icon(1,img_sound,total_buttons,total_columns);
  img_sound_x=arr[0];
  img_sound_y=arr[1];
  img_width=arr[2];
  img_height=arr[3];
  arr=draw_icon(2,img_video,total_buttons,total_columns);
  img_video_x=arr[0];
  img_video_y=arr[1];
  img_width=arr[2];
  img_height=arr[3];
  arr=draw_icon(3,img_video_recording,total_buttons,total_columns);
  img_video_recording_x=arr[0];
  img_video_recording_y=arr[1];
  img_width=arr[2];
  img_height=arr[3];
}

void home_profile()
{
draw_controls_home();  
}

void monitor_home_profile()
{
draw_controls_home();  
}