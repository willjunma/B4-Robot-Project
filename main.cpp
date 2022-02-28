#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#define COUNTSPERINCH 40.49
#define REDLIGHT .2
#define BLUELIGHT .4 
#define NOLIGHT 2.5

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
AnalogInputPin cds(FEHIO::P1_0);

void move_forward(int percent, int counts)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts, 
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_left(int percent, int counts)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.Stop();

    //While the right encoder is less than counts, keep running motors
    while(right_encoder.Counts() < counts);

    //Turn of motors
    right_motor.Stop();
    left_motor.Stop();


}

void turn_right(int percent, int counts)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.Stop();
    left_motor.SetPercent(percent);

    //While the right encoder is less than counts, keep running motors
    while(left_encoder.Counts() < counts);

    //Turn of motors
    right_motor.Stop();
    left_motor.Stop();
}

void jukebox_task(float light_value) {
    //Set percent power level and initial theoretical counts
    int motor_percent = -25;
    int expected_counts = 19.5 * COUNTSPERINCH;
    int turn_distance = (6.283185+12.56637+0.2) * COUNTSPERINCH;
    
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);
    
    //Move forward 19.5 inches then turn left 135 degrees
    move_forward(motor_percent, expected_counts);
    turn_left(motor_percent,turn_distance);

    //Move backward 2.5 inches
    motor_percent = 25;
    expected_counts = 2.5 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    //Reset motor percent
    motor_percent = -25;

    //Detect if jukebox color is red
    if ((light_value < 0.3)) {
        //Display light color
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);
        LCD.WriteLine("The color of light is red.");
        Sleep(1.0);

        //Move forward to hit button
        expected_counts = 7 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);

        //Set bot to move backwards
        motor_percent = 25;
        LCD.Clear(BLACK);

    } else if (light_value > 0.4){
        //Display light color
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);
        LCD.WriteLine("The color of light is blue.");
        Sleep(1.0);

        //Move forward to hit button
        expected_counts = 7 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        
        //Set bot to move backwards
        motor_percent = 25;
        LCD.Clear(BLACK);
    }

    //Move backwards 6 inches
    expected_counts = 6 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    //Turn left 135 degrees
    turn_distance = 18.849555 * COUNTSPERINCH;
    motor_percent = -25;
    turn_left(motor_percent, turn_distance);

    //Move forward 0.75 inches and turn left 45 degrees
    expected_counts = 0.75 * COUNTSPERINCH;
    turn_distance = 6.85 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);
    turn_left(motor_percent, turn_distance);

    //Move forward 25 inches
    motor_percent = -25;
    expected_counts = 25 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    //Move backwards 25 inches
    motor_percent = 25;
    move_forward(motor_percent, expected_counts);


}

int main(void)
{

    while (1){
    //Constantly reads the CDS value
    float light_value = cds.Value();

    //Begins if light is red
    if (light_value < BLUELIGHT) {
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);

        //Displays the color of the light
        LCD.WriteLine("The color of light is red.");
        Sleep(1.0);

        //Begins the jukebox task
        while (1) {
            jukebox_task(light_value);
            break;
        }

        //Display that the task has completed
        Sleep(2.0);
        LCD.Clear(BLACK);
        LCD.WriteLine("The task has completed");
        }
        }
    
    return 0;
}

