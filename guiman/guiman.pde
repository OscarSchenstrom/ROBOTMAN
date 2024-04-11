/*
    ROBOT MAN - 3D Visualization 
    Written by Joaquin Martinez, HiQ
*/

import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

int baud_rate = 9600;

Serial myPort;
String data="";
String serial_print_data="";
String[] serial_print_data_array = new String[20];
String keyboard_input="";
float temp, roll, pitch, yaw, acc_error_x, acc_error_y, acc_error_z,  
      gyro_error_x, gyro_error_y, gyro_error_z, distance;
PShape skeletor;
PImage hiQ_logo;
PFont ocra_font, japanese_font;

//data1, data2 and data3 store the graph information, and must be as long has the width of the screen (pixels)
float[] data1 = new float[width];
float[] data2 = new float[width];
float[] data3 = new float[width];
String str1;
String str2 = "COM7";
String[] ports = Serial.list();
boolean serial_init = false;

void setup() {
  fullScreen(P3D, 1);
  frameRate(100);
  smooth();

  temp = -1;
  distance = -1;

  if(ports.length != 0){
    serial_init = true;

    str1 = Serial.list()[0].toString();
    printArray("Connected to port: " + str1);

    myPort = new Serial(this, str1, baud_rate); // starts the serial communication

    if (myPort.available() > 0){
        myPort.bufferUntil('\n');
    }
  }
  else{  printArray("No detectable serial port!"); }

  skeletor = loadShape("assets/skeleton.obj");
  skeletor.translate(skeletor.width/2, skeletor.height/2);
  skeletor.rotateX(3);
  skeletor.rotateY(3.2);
  skeletor.scale(height/11);

  hiQ_logo = loadImage("assets/hiq.jpg");
  ocra_font = createFont("assets/OCRAEXT.TTF",30);
  japanese_font = createFont("assets/HKNipponika-Regular.ttf",260);

  delay(10);
}


//Draw the 3D-model
void draw() {

  if(ports.length != 0) {
    //Serial.list().empty();
    printArray("Port is : " + Serial.list()[0].toString());
    
    if (!serial_init){
      serial_init = true;

      str1 = Serial.list()[0].toString();
      printArray("Connected to port: " + str1);

      myPort = new Serial(this, str1, baud_rate); // starts the serial communication

      if (myPort.available() > 0){
          myPort.bufferUntil('\n');
      }
    }
  }
  else {
    serial_init = false;
    //printArray("Port length: " + ports.length);
  }
  ports = Serial.list();

  //Initial window settings
  translate(width/2, height/1.5, 0);
  background(255);

  //HiQ logo
  image(hiQ_logo,width/4,-height/1.6, width/5,height/4);

  //Text colour and font (Robotman logo) 
  fill(255, 0, 0); //Red
  textFont(japanese_font);

  //Robot man (japanese) logo
  textSize(120);
  text("ロ\nボ\nッ\nト\nマ\nン", -width/2.5, -height/2); //characters look dumb because processing doesn't support japanese characters. It says "robot man".

  textFont(ocra_font);
  textSize(20);

  //Well... ligths..
  lights();
  
  if (new_data_received) {
    display_warning_text();
    new_data_received = false;
  }

  //Text font and colour for sensor data
  fill(224,33,138); //barbie pink
  text("Temperature: ", width/4 -200, height/50 -300);
  if (temp != -1) {
    text(int(temp) + "ºC", width/4 -50, height/50 -300);
  }
  
  text("Distance: ", width/4 -200, height/50 -250);
  if (distance != -1) {
    text(int(distance) + "cm", width/4 -80, height/50 -250);
  }

  //Orientation
  fill(0, 0, 255); //BLUE
  text("Roll: " + int(roll), width/4 -200, height/50 -200);
  fill(255, 0, 0); //RED
  text("Pitch: " + int(pitch), width/4, height/50 -200);
  fill(0, 255, 0); //GREEN
  text("Yaw: " + int(yaw), width/4 +200, height/50 -200);

  //Graph
  stroke(0,0,0);                                                   //set the stroke (line) color to black
  strokeWeight(4);                                                 //set the stroke width (weight) for the axes
  line(200,200,200,200);                                          //draw the x-axis line            
  line(width/4,0,width/4,400); 

  for(int i = 0; i < 49; i++)                                 //each interation of draw, shift data points one pixel to the left to simulate a continuously moving graph
  {
    data1[i] = data1[i+1];
    data2[i] = data2[i+1];
    data3[i] = data3[i+1];
  }

  data1[49] = pitch;                                 //introduce the bufffered data into the rightmost data slot
  data2[49] = yaw;
  data3[49] = roll;

  int scale = 30;
  for(int i = 49; i > 0; i--)                                
  {
    stroke(255,0,0);
    line(i*scale - scale, 200 + data1[i-1]*-1, i*scale + scale, 200 + data1[i]*-1);
    stroke(0,255,0);
    line(i*scale - scale, 200 + data2[i-1]*-1, i*scale + scale, 200 + data2[i]*-1);
    stroke(0,0,255);
    line(i*scale - scale, 200 + data3[i-1]*-1, i*scale + scale, 200 + data3[i]*-1); 
  }

  int text_size = 50;
  if (serial_init){
    printArray(serial_print_data_array.length);
    printArray(serial_print_data_array[0]);
 
    for (int i = 0; i < serial_print_data_array.length; i++) {
      if(serial_print_data_array[i] != null) {
        text(serial_print_data_array[i], width/4 -1100, height*0.25 - i * text_size);
      }
    }
    if(DEBUG_MODE) {
      text(serial_print_data_array[0], width/4 -1100, height*0.25);
    }
  }

  text("C: " + keyboard_input,  width/4 -1100, height*0.30);

  translate(skeletor.width/2, skeletor.height/2);
  //Rotation
  rotateX(radians(pitch));
  rotateY(radians(yaw));
  rotateZ(radians(roll));

  //3D 0bject
  shape(skeletor);
}

void display_warning_text() {
  String warning_text = "";
  if (temp > 35) {
    fill(255, 0, 0);
    warning_text = "Robot is too hot!! :("; 
  }  
  else if (temp <= 20 && temp > 5) {
    warning_text = "Remove the Ice-Cream Jocke!! >:("; 
  }
  else if (temp < 5 && temp != -1) {
    fill(0, 0, 255);
  }

  if (warning_text != "") {
    text(warning_text, width/4 +50, height/50 - 300);
  }

  //Distance
  if (distance < 30 && distance != -1) {
    fill(255, 0, 0); //Object too close
    text("Object too close!!", width/4 +50, height/50 - 250);
  }
}

void keyReleased() {
  if (serial_init) {
    myPort.write(key);
  }
  keyboard_input += key;
  if (key == '\n'){
    keyboard_input = "";
  }  
}

boolean DEBUG_MODE = false;
boolean new_data_received = false;

// Read data from the Serial Port
void serialEvent (Serial myPort) { 
  // reads the data from the Serial Port up to the character '\n' and puts it into the String variable "data".
  if (myPort.available() > 0) {
      data = myPort.readStringUntil('\n');
      new_data_received = true;
  } else {
    data = null;
  }

  // if you got any bytes other than the linefeed:
  if (data != null) {
    serial_print_data = data; // + '\n';
    if (!DEBUG_MODE) {
      for (int i = serial_print_data_array.length - 1; i > 0; i--) {
        serial_print_data_array[i] = serial_print_data_array[i-1];
      }
    }
    serial_print_data_array[0] = data; // + '\n';
    if (data.contains("DEBUG_MODE off")) {
      DEBUG_MODE = false;
    } else if (data.contains("DEBUG_MODE on")) {
      DEBUG_MODE = true;      
    }

    // split the string at "/"
    String items[] = split(data, '/');
    if (items.length > 1) {
      //--- Temperature in degrees (Celsius)
      temp = float(items[0]);
      //--- Roll, Pitch and yaw in radians
      yaw = float(items[1]);
      roll = float(items[2]);
      pitch = float(items[3]);

      //--- Distance in cm
      distance = float(items[4]);
    }
  }
}
