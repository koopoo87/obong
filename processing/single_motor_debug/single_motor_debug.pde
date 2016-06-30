import processing.serial.*;
Serial myPort; 
import controlP5.*;
import java.util.*;

ControlP5 cp5;
int check = 0;
Slider s_set, s_speed;
Slider s_play;
DropdownList d1;
Button save;
int i = 0;
int listNum = 0;

boolean firstContact = false; // Whether we've heard from the microcontroller

//int cnt = 0;
String parse = "";
int servoAngle = 0;
int speed = 0;
//int movingMode = 1;

String serialPosNum = new String(); // where we will receive the values
int serialCount = 0 ; //A count of how many bytes we receive
int value = 0;

CallbackListener cb;


void setup(){
  
  size(300,400);
  //background(0);
  cp5 = new ControlP5 (this);
  String portName = Serial.list()[1];
  //Setting up the serial connection through the chosen port
  myPort = new Serial(this, portName, 9600);
  
  //dont generate a serialEvent() unless you get a line character
  myPort.bufferUntil('\n');
  serialPosNum = str(0);
 
  s_set = cp5.addSlider("servoAngle", 0, 180, servoAngle, 20, 10, 180, 20);
  s_set.setId(2);
  s_play = cp5.addSlider("p_servoAngle", 0, 180, parseInt(serialPosNum), 20, 40, 180, 20);
  s_play.setColorForeground(color(255,0,255)).setColorActive(color(155)).setColorBackground(color(128));
  
  s_speed = cp5.addSlider("speed", 1, 10, parseInt(serialPosNum), 20, 250, 180, 20);
  
  
  
  d1 = cp5.addDropdownList("easeInOut").setPosition(20,120).setSize(180,100).setId(1);
  customize(d1);
  
  /**Call back listner **/
  cb = new CallbackListener()
  {
    public void controlEvent(CallbackEvent theEvent)
    {
       //println("callbackEvent");
        if(theEvent.getAction() == ControlP5.ACTION_RELEASED) 
         {
           parse = "m:"+servoAngle+"_"+ listNum+ "_"+speed; 
                   //move all (numbers)
           println("send: "+ parse);
           myPort.write(parse);
         }
    }
  };
  
  // callback listener only reacts to s1 - s6 
  s_set.addCallback(cb);
  //cp5.addCallback(cb);
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
         inByte = trim(inByte);
         println(inByte);
         if(!inByte.equals("m"))
         {
             println("inbyte is not m");println(inByte);
             serialPosNum = inByte;
         }
      }
    
    //println("value set" + check++);
    s_play.setValue(int(serialPosNum)); 
  }
  