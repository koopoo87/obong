import processing.serial.*;
Serial myPort; 
import controlP5.*;
import java.util.*;

ControlP5 cp5;
Slider slider_speed;
Button save, restore, add, delete;
DropdownList d1;

int cnt = 0;
int speed = 1;
int list_num = 0;
String parse = "";


void setup(){
  
  size(400,400);
  cp5 = new ControlP5 (this);
  String portName = Serial.list()[1];
  //println("portName: "+portName);
  
  //Setting up the serial connection through the chosen port
  myPort = new Serial(this, portName, 9600);
  
  //dont generate a serialEven() unless you get a line character
  myPort.bufferUntil('\n');

  slider_speed = cp5.addSlider("speed", 1, 10, speed, 20, 20, 180, 20);
  d1 = cp5.addDropdownList("StatusList")
                   .setPosition(20,80)
                   .setSize(250,200)
                   .setId(1);
                   
  customize(d1);
                  
  add = cp5.addButton("+")
           .setPosition(271,80)
           .setSize(30,30)
           .setId(2);
           
  add = cp5.addButton("-")
           .setPosition(302,80)
           .setSize(30,30)
           .setId(3);
           
  save = cp5.addButton("save")
            .setPosition(271,20)
            .setSize(50,20)
            .setId(4);
            
  restore = cp5.addButton("play")
            .setPosition(322,20)
            .setSize(50,20)
            .setId(5);
}

void customize(DropdownList ddl){
  ddl.setBackgroundColor(color(190));
  ddl.setItemHeight(30);
  ddl.setBarHeight(30);

  
  for(int i =0 ; i<5; i++)
  {
    ddl.addItem("status "+i, i*2);
  }
  
  //ddl.set(array);
  ddl.setColorBackground(color(60));
  ddl.setColorActive(color(255,128));
}


public void controlEvent(ControlEvent theEvent){
  
  if(theEvent.isGroup()){
     // check if the Event was triggered from a ControlGroup
     println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());
  }
  
  if(theEvent.getController().getName().equals("save")) 
   {
     parse = "s_" + list_num + "_" + speed + "";
     parse = "save:"+parse;
     println(parse);
     myPort.write(parse);
     
   }
  if(theEvent.getController().getName().equals("play"))
   { 
     parse = "s_" + list_num + "_" + speed + "";
     parse = "play:"+parse;
     println(parse);
     myPort.write(parse);
   }
   
   
    if(theEvent.getController().getName().equals("+"))
   { 
     //println("Play Started");
      ///if(d1.getItems().size() < 6) //max : 6
      d1.addItem("status "+ d1.getItems().size(), d1.getItems().size());
    }
    
   if(theEvent.getController().getName().equals("-"))
   { 
      d1.removeItem("status "+ (d1.getItems().size()-1));
    }
   
   if(theEvent.getId()==1){
     //called from DropboxList
     //println("event from controller : "+theEvent.getController().getValue()+" from "+theEvent.getId());
     list_num = (int)theEvent.getController().getValue();
     //println("list number: " + list_num);
   }
   
}

void draw(){
   //println("speed : " + speed);
   background(128); 
}