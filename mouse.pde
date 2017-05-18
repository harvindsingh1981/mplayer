/*Check if mouse is over any icon and handle mouse events*/

void mouse_hover()
{
if(mouseX>play_pause_x && mouseY>play_pause_y && mouseX<play_pause_x+control_width && mouseY<play_pause_y+control_height)
  {
    text("play_pause",100,100);
  }

if(mouseX>stop_x && mouseY>stop_y && mouseX<stop_x+control_width && mouseY<stop_y+control_height)
  {
    text("stop",100,100);
  }

if(mouseX>volume_x && mouseY>volume_y && mouseX<volume_x+control_width && mouseY<volume_y+control_height)
  {
    text("volume",100,100);
  }

if(mouseX>next_x && mouseY>next_y && mouseX<next_x+control_width && mouseY<next_y+control_height)
  {
    text("next",100,100);
  }

if(mouseX>previous_x && mouseY>previous_y && mouseX<previous_x+control_width && mouseY<previous_y+control_height)
  {
    text("previous",100,100);
   }
}

void mousePressed() 
{
if(mouseX>play_pause_x && mouseY>play_pause_y && mouseX<play_pause_x+control_width && mouseY<play_pause_y+control_height)
  {
    text("play_pause Pressed",100,100);
    if(state=="PLAY")
    {
      player.pause();
      state="PAUSE";
    }
    else if(state=="PAUSE")
    {
      player.play();
      state="PLAY";
    }
    else
    {
      load_audio_file(audio_files[current_audio_track]);
      display_audio_info();
      player.play();
      state="PLAY";
    }
  }

if(mouseX>stop_x && mouseY>stop_y && mouseX<stop_x+control_width && mouseY<stop_y+control_height)
  {
    player.close();
    state="STOP";
    text("stop  Pressed",100,100);
  }

if(mouseX>volume_x && mouseY>volume_y && mouseX<volume_x+control_width && mouseY<volume_y+control_height)
  {
    text("volume  Pressed",100,100);
  }

if(mouseX>next_x && mouseY>next_y && mouseX<next_x+control_width && mouseY<next_y+control_height)
  {
    if(current_audio_track<total_audio_songs-1)
    {
      player.close();
      current_audio_track++;
      load_audio_file(audio_files[current_audio_track]);
      display_audio_info();
      player.play();
      state="PLAY";
    }
    text("next  Pressed",100,100);
  }

if(mouseX>previous_x && mouseY>previous_y && mouseX<previous_x+control_width && mouseY<previous_y+control_height)
  {
    if(current_audio_track>0)
    {
      player.close();
      current_audio_track--;
      load_audio_file(audio_files[current_audio_track]);
      display_audio_info();
      player.play();
      state="PLAY";
    }
    text("previous  Pressed",100,100);
   }
if(mouseX>slider_x && mouseY>slider_y && mouseX<slider_x+slider_width && mouseY<slider_y+slider_height)
  {
    float clicked_pos;
    clicked_pos=map(mouseX,slider_x,slider_x+slider_width,0,player.length());
//    player.cue((int)clicked_pos);
//    player.pause();
//    player.skip((int)clicked_pos-player.position());
    player.play((int)clicked_pos);
    println("skipped !!!!");
//    text("slider  Pressed",100,100);
  }
}