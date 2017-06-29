/*
Different Profiles for Audio, Video and Image viewer
Default profile is Audio
*/

int total_audio_songs = 0;
int current_audio_track = 0;
void audio_profile()
{
  find_audio_files("/home/pi/Downloads/sonu_nigam/");
  total_audio_songs=audio_files.length;
  println(total_audio_songs);
  load_audio_file(audio_files[0]);
  display_audio_info();
  player.play();
  state="PLAY";
  current_audio_track=0;
}

void monitor_audio_profile()
{
  draw_controls_audio();
  float ball_current_pos=ball_x;
  display_audio_info();
  audio_visulizer();
//    audio_visulizer_bar();
//    rect(horizontal_padding,height-vertical_padding-control_height-50,slider_width,slider_height);//slider    
    text(player.position()/60000 + ":" + nf(player.position()%60000/1000,2) + "/" + player.length()/60000 + ":" + player.length()%60000/1000, horizontal_padding,height-vertical_padding-control_height-50+slider_height+20);
    ball_current_pos=map(player.position(),0,player.length(),0+horizontal_padding-(ball_width/2),width-horizontal_padding-(ball_width/2));
    fill(0,255,0);
    rect(slider_x,slider_y,slider_width,slider_height);
    noFill();

    fill(255,0,0);
    rect(slider_x,slider_y,ball_current_pos,slider_height);
    noFill();

    image(img_ball,ball_current_pos,height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2),ball_width,ball_height);//slider ball
    if(!player.isPlaying() && (current_audio_track<total_audio_songs-1) && state=="PLAY")
    {
      current_audio_track++;
      load_audio_file(audio_files[current_audio_track]);
      display_audio_info();
      player.play();
      state="PLAY";
    }
}

void draw_controls_audio()
{
int total_horizontal_controls=3;
int spacing = (width-((control_width*total_horizontal_controls)/*+(padding*2)*/))/(total_horizontal_controls+1);
//int slider_height=5;
//int slider_width=width-(horizontal_padding*2);

control_width=80;
control_height=80;

image(img_background,0,0,width,height);

image(img_previous,0+10,(height/2)-(control_height/2),control_width,control_height);
previous_x=0+10;
previous_y=(height/2)-(control_height/2);

image(img_next,width-control_width-10,(height/2)-(control_height/2),control_width,control_height);
next_x=width-control_width-10;
next_y=(height/2)-(control_height/2);

//image(img_play,padding,height-padding-control_height,control_width,control_height);
image(img_play_pause,spacing,height-vertical_padding-control_height,control_width,control_height);
play_pause_x=spacing;
play_pause_y=height-vertical_padding-control_height;
//image(img_pause,padding+(control_width*1)+(spacing*1),height-padding-control_height,control_width,control_height);

image(img_stop,spacing+(control_width*1)+(spacing*1),height-vertical_padding-control_height,control_width,control_height);
stop_x=spacing+(control_width*1)+(spacing*1);
stop_y=height-vertical_padding-control_height;

image(img_volume,spacing+(control_width*2)+(spacing*2),height-vertical_padding-control_height,control_width,control_height);
volume_x=spacing+(control_width*2)+(spacing*2);
volume_y=height-vertical_padding-control_height;

fill(0,255,0);
rect(horizontal_padding,height-vertical_padding-control_height-50,slider_width,slider_height);//slider
noFill();
slider_x=horizontal_padding;
slider_y=height-vertical_padding-control_height-50;

//image(img_ball,horizontal_padding-(ball_width/2),height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2),ball_width,ball_height);//slider ball
ball_x=horizontal_padding-(ball_width/2);
ball_y=height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2);
//println("Draw Controls......Done!");

image(img_home,horizontal_padding+350+175,vertical_padding,home_width,home_height);
home_x=horizontal_padding+350+175;
home_y=vertical_padding;
}