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