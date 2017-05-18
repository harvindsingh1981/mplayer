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
  float ball_current_pos=ball_x;
    stroke(255);
    display_audio_info();
    audio_visulizer();
    audio_visulizer_bar();
//    rect(horizontal_padding,height-vertical_padding-control_height-50,slider_width,slider_height);//slider    
    text(player.position()/60000 + ":" + nf(player.position()%60000/1000,2) + "/" + player.length()/60000 + ":" + player.length()%60000/1000, horizontal_padding,height-vertical_padding-control_height-50+slider_height+20);
    ball_current_pos=map(player.position(),0,player.length(),0+horizontal_padding-(ball_width/2),width-horizontal_padding-(ball_width/2));
    fill(0,255,0);
    rect(slider_x,slider_y,slider_width,slider_height);
    fill(255,0,0);
    rect(slider_x,slider_y,ball_current_pos,slider_height);
    noFill();
    image(img_ball,ball_current_pos,height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2),ball_width,ball_height);//slider ball
    if(!player.isPlaying() && (current_audio_track<total_audio_songs-1))
    {
      current_audio_track++;
      load_audio_file(audio_files[current_audio_track]);
      display_audio_info();
      player.play();
      state="PLAY";
    }
}