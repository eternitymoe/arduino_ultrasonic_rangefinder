/*
Arduino ultrasonic rangefinder Source code

Created on November 16 2022 by eternitymoe; Modified on November 17 2022 07:37 EST;
This project is licensed under the MIT license (https://gist.github.com/eternitymoe/88bb61cfe5665da47f00ab8e5f240d89)

Components:
-   Arduino Uno Mini
-   HC-SR04 Ultrasonic Sensor
-   16x2 Character LCD
*/

#include <Arduino.h> // including the Main include file for the Arduino SDK
#include <LiquidCrystal.h> // including the LiquidCrystal library that allows Arduino to control Character LCDs

LiquidCrystal lcdScr(13, 12, 11, 10, 9, 8); // set Pins to LCD Screen (global class)
int trigPin = 7; // # of Pin to the TRIG pin of Ultrasonic Sensor (global variable)
int echoPin = 6; // # of Pin to the ECHO pin of Ultrasonic Sensor (global variable)

void setup() { // built-in setup() function for start-up initiation codes
    Serial.begin(115200); // set Serial port baud rate (!!! The Arduino will hang when the Serial Monitor is not connected)
    lcdScr.begin(16, 2); // set LCD Screen size
    pinMode(trigPin, OUTPUT); // set the Pin to the TRIGGER (TX) Pin of the Ultrasonic Sensor to OUTPUT mode 
    pinMode(echoPin, INPUT); // set the Pin to the ECHO (RX) Pin of the Ultrasonic Sensor to OUTPUT mode
}

double roundDecimal2p(double originalValue) { // function for rounding original distance value to 2 decimal places
    return (long)(originalValue * 100 + 0.5) / 100.0; // return rounded value
}

double ultrasonicMeasure() { // function for measuring ultrasonic travel time (in μs)
    digitalWrite(trigPin, LOW); // turn off the ultrasound sender
    delayMicroseconds(2); // delay 2 μs before turn on the sender
    digitalWrite(trigPin, HIGH); // turn on the ultrasound sender (launch acoustic burst)
    delayMicroseconds(10); // continue launching acoustic burst for 10 μs
    digitalWrite(trigPin, LOW); // turn off the ultrasound sender
    return pulseIn(echoPin, HIGH); // return the value of "duration taken for acoustic burst return to the ultrasound receiver"
}

void distanceOutput(double echoDuration) { // function for outputing distance to Serial Monitor and LCD Screen
    double distanceCm = (echoDuration / 2) * 0.0343; // calculate distance in centimeters
    double distanceIn = (echoDuration / 2) * 0.0135039; // calculate distance in inches
    if (distanceCm > 400 || distanceCm < 2){ // determine is the value in the measuring range
        String outofrangeError = ("Measuring range is 2 ~ 400 cm, but got " + (String)distanceCm); // concatenate the string for out-of-range error message
        if (distanceCm != 0) {  // if the distance value reported is not 0, the sensor should be working but out of measuring range
            Serial.println("[+" + (String)millis() + "ms] Out of range! " + outofrangeError); // print out-of-range error message on the Serial Monitor
            lcdscrollingOutputl2("Out of range!", ("Range is 2-400cm,But got ~" + (String)(roundDecimal2p(distanceCm)) + "cm")); // call lcdscrollingOutputl2() function to print a scrolling error message on the LCD Screen
        }
        else { // the distance value reported 0, the sensor is probably not connected or doesn't working
            Serial.println("[+" + (String)millis() + "ms] Out of range! " + outofrangeError + " (Is it connected?)"); // print out-of-range error message with a notice about the abnormal distance value on the Serial Monitor
            lcdscrollingOutputl2("Out of range!", "Check the sensor connection!"); // call lcdscrollingOutputl2() function to print a scrolling error message on the LCD Screen
        }
    }
    else { // the distance value reported seems in measuring range, printing it
        Serial.println("[+" + (String)millis() + "ms] Measured distance: " + (String)distanceCm + " cm / " + (String)distanceIn + " inches");  // print the log of measured distance on the Serial Monitor
        lcdScr.clear(); // clear all characters on the LCD Screen
        lcdScr.setCursor(0, 0); // let the LCD Screen start print at the 1st-row 1st-character position
        lcdScr.print(roundDecimal2p(distanceCm)); // print rounded distance value in centimeters on the LCD Screen
        lcdScr.print(" cm"); // append (print) "cm" after distance value on the LCD Screen
        lcdScr.setCursor(0, 1); // let the LCD Screen start print at the 2nd-row 1st-character position
        lcdScr.print(roundDecimal2p(distanceIn)); // print rounded distance value in inches on the LCD Screen
        lcdScr.print(" in"); // append (print) "inches" after distance value on the LCD Screen
    }
}

// original code by dedekeyser (https://forum.arduino.cc/t/16x2-lcd-one-line-scrolling-one-stationary/406501)
void lcdscrollingOutputl2(String line1Text, String line2Text) { // function for printing a scrolling message on the LCD Screen (only used for error messages due to it takes too much cpu time)
    lcdScr.clear(); // clear all characters on the LCD Screen
    lcdScr.setCursor(0, 0); // let the LCD Screen start print at the 1st-row 1st-character position
    lcdScr.print(line1Text); // print out-of-range warning on the LCD Screen
    if ((line2Text.length()) <= 16) { // if the length of line2 is not longer than 16, print regularly
        lcdScr.setCursor(0, 1); // let the LCD Screen start print at the 2nd-row 1st-character position
        lcdScr.print(line2Text); // print line2 regularly
    }
    else if ((line2Text.length()) > 39) { // (!!!) Due to RAM constraints, strings longer than 39 characters are too buggy to be output to the display
        Serial.println("[Error] Line 2 string is too long (> 39) to scrolling print on the LCD Screen"); // print the error message about too long string on Serial Monitor
    }
    else { // when 16 < length of line2 < 40, scroll the line2
        char line2charArray[40]; // create a character list of the same length of error message line2 string 
        lcdScr.setCursor(0, 1); // let the LCD Screen start print at the 2nd-row 1st-character position
        line2Text.toCharArray(line2charArray, 40); // put the error message from the string into the character list
        for (int i = 0; i <= 16; i++) { // printing first 16 characters
            lcdScr.write(line2charArray[i]); // print the first 16 characters to the LCD Screen
        }
        delay(100); // delay 100 ms for reading the text
        for (int j = 17; j <= (line2Text.length()); j++) { // begin printing from character #17 onward
            lcdScr.write(line2charArray[j]); // write the j-th character (for now it will be off-screen).
            lcdScr.scrollDisplayLeft(); // scroll the text left one character-space.
            lcdScr.setCursor(j-16,0); // set the cursor to the (visually) 1st-character slot on the 1st-row
            lcdScr.print(line1Text); // re-print the row 1 message
            lcdScr.setCursor(j+1,1); // set the cursor one character space to the right of the last printed character on row 2
            delay(100); // delay 100 ms for reading the text
        }
    }
}

void loop() { // built-in loop() function for codes to looping
    distanceOutput(ultrasonicMeasure()); // call functions for a measurement attempt
    delay(100); // delay 100 ms between 2 measurement attempts
}
