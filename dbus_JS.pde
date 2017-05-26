
import javax.script.*;
ScriptEngineManager manager = new ScriptEngineManager();
ScriptEngine engine = manager.getEngineByName("JavaScript");
void run_js()
{
    try
    {
      engine.eval("print('Hello World');");
    }
    catch(ScriptException e)
    {
      e.printStackTrace();
    }
}