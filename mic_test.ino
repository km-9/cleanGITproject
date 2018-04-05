
#define LIN_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
#include <FFT.h>


void setup() {

  Serial.begin(9600);

  sound();
  Serial.println("sound");
  delay(3000);

}

void loop() {
  //Serial.println(analogRead(15)-512);
  //delay(2);
}

void sound() {
  read_sound();
  int low_frequency_counter = 0;
  while(low_frequency_counter < 10) {
    if (low_frequency_sum() > 140)
      low_frequency_counter++;
    else
      low_frequency_counter = 0;
    read_sound();
    //display_dominant_bins();
  }
}

void read_sound() {
  for(int i=0; i < 512; i += 2) 
  {
   // Serial.println(i);
    fft_input[i] = analogRead(5)-512;
    fft_input[i+1] = 0;
  }
  // window data, then reorder, then run, then take output
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_lin(); // take the output of the fft
}

int low_frequency_sum() {
  int sum = 0;

  for(int i = 90; i<125; i++)
    sum+=fft_lin_out[i];
    
  return (sum);
}

void display_sound() {
  for(int i=0; i < 512; i += 2) {
    Serial.println(fft_input[i]);
  }
}

void display_dominant_bins() {
  for(int i=0; i < FFT_N/2; i++) {
     if (fft_lin_out[i] > 10)  {
      Serial.print(i);
      Serial.print(" ");
     }
  }
  Serial.println();
  delay(1000);
}

void display_fft() {
      for(int i=20; i < 110; i++) {
      Serial.print(i);
      Serial.print(": ");
      histogram_bar(fft_lin_out[i]);

    }
    delay(1000);

}

void histogram_bar(int v) {
  for(int i=0; i < v/5; i++) Serial.print("*");
  Serial.println();
}



