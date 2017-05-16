/*
itterate through given root directory and seggeregates audio, video and Image files
*/

String root_directory="";
String[] result;//=new String[20];

void find_audio_files(String path)
{
  File[] files = listFiles(path);
  int j=0; //<>// //<>//
  String file_type=".mp3";

  for(int i=0;i<files.length;i++)if(files[i].getName().endsWith(file_type))j++;
  result=new String[j];

  j=0;
  for(int i=0;i<files.length;i++)
  {
    if(files[i].getName().endsWith(file_type))
    {
//      result[j]=files[i].getName();
      result[j]=files[i].getAbsolutePath();
      j++;
    }
  }
//printArray(result);
//  return result;
}