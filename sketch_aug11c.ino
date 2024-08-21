#include <ArduinoSTL.h>
#include <Stepper.h>

// Define pins
int constexpr reverseSwitch = 2; // Push button for reverse
int constexpr driverDIR     = 6; // DIR – pin
int constexpr driverPUL     = 7; // PUL pin
int constexpr startPORT     = 8; // variable => power on --> Run program, power off --> disable
int constexpr fastRIGHT     = 9; // variable => power on --> Run conti. right; power off --> unuseful

// Variables
int constexpr waitpd = 1000*5; // collect sample time in millisecond
int constexpr pd     = 50;     // Pulse Delay period in millisecond, pulse/pulse duration
int constexpr frpd   = 500;    // fast right
bool          setdir = LOW;    // Set Direction
int           wpd    = waitpd; // initial collection/waiting time

//Interrupt Handler
void revmotor ()
{
    setdir = !setdir;
}
void fastspeed ()
{
    wpd = frpd;
}
void samplespeed ()
{
    wpd = waitpd;
}

void setup()
{
    Serial.begin(9600);
    pinMode (driverDIR, OUTPUT);
    pinMode (driverPUL, OUTPUT);
    pinMode (startPORT, INPUT);
    pinMode (fastRIGHT, INPUT);
    attachInterrupt(digitalPinToInterrupt(reverseSwitch), revmotor, FALLING);
    attachInterrupt(digitalPinToInterrupt(fastRIGHT), fastspeed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(fastRIGHT), samplespeed, CHANGE);
}
void loop()
{
    for (int i = 0; i < 200*100; i++)
    {
        digitalWrite(driverDIR, setdir);
        digitalWrite(driverPUL, HIGH);
        delayMicroseconds(pd);
        digitalWrite(driverPUL, LOW);
        delayMicroseconds(pd);
    }
    delay(wpd);
}
