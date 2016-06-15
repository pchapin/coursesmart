#include "message.h"

void Message::Message()
{
 List<string> text;
 char buffer[256];

 if (Get_NTPObject(oid_t)){
   while(Get_NPTData(buffer,256)!=0){
     text.Append(buffer);
   }
 return;
}


oid_t Message::Interact()
{
  FILE *fp;
  String line;

  fp=popen("more", "w");
  text.Start_Iteration();
  if (text.Number_of_Items()>0){
    do{
      line=text.Get_Current();
      fprintf(fp,"%s",(char *)line);
      text.Next();
    }while(text.End()!=1);
    return 0;
  }
  return 1;                             //return 1 if the message is empty
}
