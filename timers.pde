import com.dhchoi.CountdownTimer;
import com.dhchoi.CountdownTimerService;
CountdownTimer timer0;
CountdownTimer timer1;
void create_countdown_timers()
{
  timer0=CountdownTimerService.getNewCountdownTimer(this).configure(2000, 2000).start();//timer for flashing recording icon
  timer1=CountdownTimerService.getNewCountdownTimer(this).configure(2000, 2000).start();//Aux Timer
}

void onTickEvent(CountdownTimer t, long timeLeftUntilFinish)
{
  
}

void onFinishEvent(CountdownTimer t)
{
  switch(t.getId())
  {
    case 0: //timer for flashing record icon
    println("[Timer"+t.getId()+"]"+"........In flashing record icon timer");
    t.reset(CountdownTimer.StopBehavior.STOP_IMMEDIATELY);
    t.start();
    break;
    case 1://Aux timer not used
    println("[Timer"+t.getId()+"]"+"........In Aux timer");
    t.reset(CountdownTimer.StopBehavior.STOP_IMMEDIATELY);
    t.start();
    break;
  }
}