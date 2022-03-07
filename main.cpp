#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#define COUNTSPERINCH 40.49
#define REDLIGHT .2
#define BLUELIGHT .4 
#define NOLIGHT 2.5

//Declarations for encoders & motors
DigitalEncoder left_encoder(FEHIO::P0_0);
DigitalEncoder right_encoder(FEHIO::P0_1);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHServo ticket_servo(FEHServo::Servo5);
FEHServo tray_servo(FEHServo::Servo7);
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

void tray_task () {
    //Set motor percent and theoretical thread counts
    int motor_percent = 25;
    int expected_counts = 7.5 * COUNTSPERINCH;
    int turn_distance = (25.13274/2) * COUNTSPERINCH;

    //Initialize tray servo 
    tray_servo.SetMin(575);
    tray_servo.SetMax(2450);
    tray_servo.SetDegree(175);
    Sleep(1.0);

    //Move forward 7.5 inches
    move_forward(motor_percent, expected_counts);

    //Turn 180 degrees
    turn_left(motor_percent, turn_distance);
    motor_percent = -25;
    turn_distance = 15 * COUNTSPERINCH;
    turn_right(motor_percent, turn_distance);

    //Move forward and then turn 45 degrees
    expected_counts = 12 * COUNTSPERINCH;
    turn_distance = 9 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);
    turn_left(motor_percent, turn_distance);

    //Move forward 2.5 inches
    expected_counts = 2.5 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);  

    //Move servo arm
    tray_servo.SetDegree(155);
    Sleep(2.0);
    tray_servo.SetDegree(175);

    //Turn to original position and then move backwards
    motor_percent = 25;
    move_forward(motor_percent, expected_counts);  
    turn_distance = 12 * COUNTSPERINCH;
    expected_counts = 5 * COUNTSPERINCH;
    turn_left(motor_percent, turn_distance);
    move_forward(motor_percent, expected_counts);

    //Turn 45 degrees and then move up the ramp
    turn_distance = 13 * COUNTSPERINCH;
    turn_right(motor_percent, turn_distance);
    motor_percent = -40;
    expected_counts = 34 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);
}

void ticket_task() {
    //Initialize the ticket servo
    ticket_servo.SetMin(550);
    ticket_servo.SetMax(2485);
    ticket_servo.SetDegree(0);

    //Set motor speed and initial turn and movement distances
    int motor_percent = 25;
    int turn_distance = 12.56637 * COUNTSPERINCH;
    int expected_counts = 2.45 * COUNTSPERINCH;

    //Turn right and move forward 2.45 inches
    turn_left(motor_percent, turn_distance);
    move_forward(motor_percent, expected_counts);

    //Turn left and move forward 2.25 inches
    turn_distance = 10.5 * COUNTSPERINCH;
    turn_right(motor_percent, turn_distance);
    expected_counts = 2.25 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    //Extend the ticket arm
    Sleep(2.0);
    ticket_servo.SetDegree(180);
    Sleep(1.0);

    //Turn left to move the ticket and retract the ticket arm
    turn_distance = 10 * COUNTSPERINCH;
    motor_percent = -40;
    turn_left(motor_percent, turn_distance);
    motor_percent = 40;
    turn_right(motor_percent, turn_distance);
    Sleep(1.0);
    ticket_servo.SetDegree(0);

    //Turn left
    motor_percent = 40;
    turn_distance = 10 * COUNTSPERINCH;
    turn_right(motor_percent, turn_distance);

    //Move robot towards burger flipper
    motor_percent = 25;
    expected_counts = 30 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);
}

int main(void)
{
    float time = TimeNow();
     while (TimeNow() - time < 360.0){
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
            tray_task();
            ticket_task();
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


