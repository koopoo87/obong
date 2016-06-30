import processing.serial.*;
Serial myPort; 
import controlP5.*;
import java.util.*;

ControlP5 cp5;
int check = 0;
Slider s1_set, s2_set, s3_set, s4_set, s5_set, s6_set, speed;
Slider s1_play, s2_play, s3_play, s4_play, s5_play, s6_play;
DropdownList d1;
Button save;
int i = 0;
int listNum = 0;

boolean firstContact = false; // Whether we've heard from the microcontroller

//int cnt = 0;
String parse = "";

int servoAngle1 = 0;
int servoAngle2 = 0;
int servoAngle3 = 0;
int servoAngle4 = 0;
int servoAngle5 = 0;
int servoAngle6 = 0;

int mSpeed = 1;

String[] serialInArray = new String[6]; // where we will receive the values
int serialCount = 0 ; //A count of how many bytes we receive
int value = 0;

CallbackListener cb;
CallbackListener speedCtrl;



void setup(){
  
  size(600,400);
  //background(0);
  cp5 = new ControlP5 (this);
  String portName = Serial.list()[1];
  //Setting up the serial connection through the chosen port
  myPort = new Serial(this, portName, 9600);
  
  //dont generate a serialEvent() unless you get a line character
  myPort.bufferUntil('\n');
  
  for(int i =0 ; i < 6 ; i++)
  {
    serialInArray[i] = str(0);
  }
 
  s1_set = cp5.addSlider("servoAngle1", 0, 180, servoAngle1, 20, 10, 180, 20);
  s1_set.setId(2);
  s1_play = cp5.addSlider("p_servoAngle1", 0, 180, parseInt(serialInArray[0]), 20, 40, 180, 20);
  s1_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  s2_set = cp5.addSlider("servoAngle2", 0, 180, servoAngle2, 20, 70, 180, 20);
  s2_set.setId(2);
  s2_play = cp5.addSlider("p_servoAngle2", 0, 180, parseInt(serialInArray[1]), 20, 100, 180, 20);
  s2_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  s3_set = cp5.addSlider("servoAngle3", 0, 180, servoAngle3, 20, 130, 180, 20);
  s3_set.setId(2);
  s3_play = cp5.addSlider("p_servoAngle3", 0, 180, parseInt(serialInArray[2]), 20, 160, 180, 20);
  s3_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  s4_set = cp5.addSlider("servoAngle4", 0, 180, servoAngle4, 20, 190, 180, 20);
  s4_set.setId(2);
  s4_play = cp5.addSlider("p_servoAngle4", 0, 180, parseInt(serialInArray[3]), 20, 220, 180, 20);
  s4_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  s5_set = cp5.addSlider("servoAngle5", 0, 180, servoAngle5, 20, 250, 180, 20);
  s5_set.setId(2);
  s5_play = cp5.addSlider("p_servoAngle5", 0, 180, parseInt(serialInArray[4]), 20, 280, 180, 20);
  s5_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  s6_set = cp5.addSlider("servoAngle6", 0, 180, servoAngle6, 20, 310, 180, 20);
  s6_set.setId(2);
  s6_play = cp5.addSlider("p_servoAngle6", 0, 180, parseInt(serialInArray[5]), 20, 340, 180, 20);
  s6_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  d1 = cp5.addDropdownList("easeInOut").setPosition(300,10).setSize(150,100).setId(1);
  customize(d1);
  
  speed =  cp5.addSlider("mSpeed", 0, 10, mSpeed, 300, 120, 150, 20);
  
  /**Call back listner **/
  cb = new CallbackListener()
  {
    public void controlEvent(CallbackEvent theEvent)
    {  
       //println("callbackEvent");
        if(theEvent.getAction() == ControlP5.ACTION_RELEASED) 
         {
           parse = "("+servoAngle1+","+servoAngle2+","+servoAngle3+","+servoAngle4+","+servoAngle5+","+servoAngle6+")" + listNum+"_"+mSpeed; 
                   //move all (numbers)
           println("send: "+ parse);
           myPort.write(parse);
         }
    }
  };

  
  // callback listener only reacts to s1 - s6 
  s1_set.addCallback(cb);
  s2_set.addCallback(cb);
  s3_set.addCallback(cb);
  s4_set.addCallback(cb);
  s5_set.addCallback(cb);
  s6_set.addCallback(cb); 
}

void customize(DropdownList ddl)
{
  ddl.setBackgroundColor(color(190));
  ddl.setItemHeight(20);
  ddl.setBarHeight(20);
  
  //addListItem
  ddl.addItem("Linear tweening", 0);
  ddl.addItem("Cubic easeInOut", 1);
  ddl.addItem("Bounce easeInOut",2);
  
  ddl.setColorBackground(color(6));
  ddl.setColorActive(color(255,128));
}


public void controlEvent(ControlEvent theEvent){

   if(theEvent.getId()==1){
   //called from DropboxList
   listNum = (int)theEvent.getController().getValue();
   println("list number: " + listNum);
   }
}

void draw(){
  
   background(128); 
   serialEvent();
   
}

void serialEvent()
{
      
      String inByte = myPort.readStringUntil(10);
      if(inByte!=null)
      {
         //print("received: "+ inByte);
         inByte = trim(inByte);
         if( inByte.equals("m") || i == 6)
           i = 0;
         
         if(!inByte.equals("m"))
         {
             //println("inbyte is not m");println(inByte);
             serialInArray[i] = inByte;
             println("i: "+ i+", "+ serialInArray[i]);//println(value);
             i++;
         }
      }
    
    //println("value set" + check++);
    s1_play.setValue(int(serialInArray[0])); 
    s2_play.setValue(int(serialInArray[1])); 
    s3_play.setValue(int(serialInArray[2])); 
    s4_play.setValue(int(serialInArray[3])); 
    s5_play.setValue(int(serialInArray[4])); 
    s6_play.setValue(int(serialInArray[5])); 

  }
  