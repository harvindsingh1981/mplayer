/*
itterate through given root directory and seggeregates audio, video and Image files
*/

String root_directory="";
String[] audio_files;//=new String[20];

void find_audio_files(String path)
{
  File[] files = listFiles(path);
  int j=0; //<>//
  String file_type=".mp3";

  for(int i=0;i<files.length;i++)if(files[i].getName().endsWith(file_type))j++;
  audio_files=new String[j];

  j=0;
  for(int i=0;i<files.length;i++)
  {
    if(files[i].getName().endsWith(file_type))
    {
//      result[j]=files[i].getName();
      audio_files[j]=files[i].getAbsolutePath();
      j++;
    }
  }
//printArray(result);
//  return result;
}