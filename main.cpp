#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#define COUNTSPERINCH 40.49
#define COUNTSPERDEGREE 5.65347023667
#define REDLIGHT .25
#define BLUELIGHT .4
#define NOLIGHT 2.5

//Declarations for encoders & motors
DigitalEncoder left_encoder(FEHIO::P0_1);
DigitalEncoder right_encoder(FEHIO::P0_0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHServo burger_servo(FEHServo::Servo7);
FEHServo ticket_servo(FEHServo::Servo5);
FEHServo tray_servo(FEHServo::Servo4);
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

void turn_right(int percent, int counts){
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

void bottom_level() {
    //Set motor percent and theoretical thread counts
    int motor_percent = 70;
    int expected_counts = 7.5 * COUNTSPERINCH;
    int turn_distance = 90 * COUNTSPERDEGREE;

    //Initialize tray servo 
    tray_servo.SetMin(575);
    tray_servo.SetMax(2450);

    //Move forward 7.5 inches
    move_forward(motor_percent, expected_counts);

    //Turn 180 degrees
    turn_left(motor_percent, turn_distance);
    motor_percent = -70;
    turn_distance = 135 * COUNTSPERDEGREE;
    turn_right(motor_percent, turn_distance);

    expected_counts = 4.0 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    //Straighten out robot
    float time = TimeNow();
        while((TimeNow() - time) < 2.5) {
            right_motor.SetPercent(-25);
            left_motor.SetPercent(-25);
        }
    right_motor.Stop();
    left_motor.Stop();

    //Turn towards trash
    motor_percent = 25;
    expected_counts = 7.0 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    motor_percent = -25;
    turn_distance = 90 * COUNTSPERDEGREE;
    turn_left(motor_percent, turn_distance);

    //Press robot against trash
    time = TimeNow();
        while((TimeNow() - time) < 1.75) {
            right_motor.SetPercent(-25);
            left_motor.SetPercent(-25);
        }
    right_motor.Stop();
    left_motor.Stop();

    //Drop the tray
    tray_servo.SetDegree(150);
    Sleep(0.5);
    tray_servo.SetDegree(180);

    motor_percent = 25;
    expected_counts = 7.6 * COUNTSPERINCH;
    turn_distance = 17 * COUNTSPERDEGREE;

    //Move towards the jukebox light
    turn_left(motor_percent, turn_distance);
    move_forward(motor_percent, expected_counts);
    Sleep(1.0);

    //Read in the jukebox light
    if(cds.Value() < BLUELIGHT) {
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);
        LCD.WriteLine("The color of light is red.");
        
        expected_counts = 1.5 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        LCD.Clear(RED);
        turn_right(motor_percent, turn_distance);

        turn_distance = 15.5 * COUNTSPERDEGREE;
        turn_left(motor_percent, turn_distance);
        
        time = TimeNow();
        while((TimeNow() - time) < 2.0) {
            right_motor.SetPercent(25);
            left_motor.SetPercent(25);
        }
        right_motor.Stop();
        left_motor.Stop();

        expected_counts = 1.75 * COUNTSPERINCH;
        motor_percent = -25;
        expected_counts = 0.5 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);

        turn_left(motor_percent, turn_distance);
        move_forward(motor_percent, expected_counts);

        expected_counts = 0.25 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_distance = 15.5 * COUNTSPERDEGREE;
        expected_counts = 1.25 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_right(motor_percent, turn_distance);

        motor_percent = 25;
        expected_counts = 3.0 * COUNTSPERINCH;
        turn_distance = 120 * COUNTSPERDEGREE;

        move_forward(motor_percent, expected_counts);
        turn_left(motor_percent, turn_distance);
        expected_counts = 3.0 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_distance = 46 * COUNTSPERDEGREE;
        turn_left(motor_percent, turn_distance);

    } else if (cds.Value() > BLUELIGHT) {
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);
        LCD.WriteLine("The color of light is blue.");

        turn_distance = 9.5 * COUNTSPERDEGREE;
        turn_left(motor_percent, turn_distance);
        LCD.Clear(BLUE);

        time = TimeNow();
        while((TimeNow() - time) < 2.0) {
            right_motor.SetPercent(25);
            left_motor.SetPercent(25);
        }
        right_motor.Stop();
        left_motor.Stop();

        expected_counts = 5.75 * COUNTSPERINCH;
        motor_percent = -25;
        move_forward(motor_percent, expected_counts);
        turn_left(motor_percent, turn_distance);

        motor_percent = 25;
        expected_counts = 3.0 * COUNTSPERINCH;
        turn_distance = 120 * COUNTSPERDEGREE;

        move_forward(motor_percent, expected_counts);
        turn_left(motor_percent, turn_distance);
        expected_counts = 3.0 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_distance = 41 * COUNTSPERDEGREE;
        turn_left(motor_percent, turn_distance);
    }


}

void burger_task() {
    int motor_percent = 60;
    int expected_counts = 30 * COUNTSPERINCH;
    
    //Initialize burger flipping servo
    burger_servo.SetMin(650);
    burger_servo.SetMax(2450);
    burger_servo.SetDegree(0);

    //Move up the robot
    move_forward(motor_percent, expected_counts);

    //Turn and press robot against the wall
    motor_percent = -25;
    int turn_distance = 90 * COUNTSPERDEGREE;
    turn_right(motor_percent, turn_distance);
    float time = TimeNow();
    while((TimeNow() - time) < 2.0) {
        right_motor.SetPercent(-40);
        left_motor.SetPercent(-40);
    }
    right_motor.Stop();
    left_motor.Stop();

    //Turn and move towards burger
    motor_percent = 25; 
    expected_counts = 3.75 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    turn_distance = 87.25 * COUNTSPERDEGREE;
    turn_right(motor_percent, turn_distance);

    time = TimeNow();
    while((TimeNow() - time) < 2.0) {
        right_motor.SetPercent(30);
        left_motor.SetPercent(30);
    }
    right_motor.Stop();
    left_motor.Stop();

    motor_percent = -25;
    expected_counts = 0.35 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    //Flip the burger
    Sleep(0.25);
    burger_servo.SetDegree(90);
    Sleep(.50);
    burger_servo.SetDegree(10);
    Sleep(0.25);

    //Turn towards to ticket servo
    expected_counts = 3 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);
    turn_distance = 90 * COUNTSPERDEGREE;
    turn_right(motor_percent, turn_distance);

    time = TimeNow();
    while((TimeNow() - time) < 1.0) {
        right_motor.SetPercent(-40);
        left_motor.SetPercent(-40);
    }
    right_motor.Stop();
    left_motor.Stop(); 

    motor_percent = 25;
    expected_counts = 4 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);

    turn_distance = 110 * COUNTSPERDEGREE;
    turn_left(motor_percent, turn_distance);
}

void ticket_task() {
    int motor_percent = 25;
    int expected_counts = 3.4 * COUNTSPERINCH;
    int turn_distance = 10 * COUNTSPERDEGREE;

    //Initialize ticket servo
    ticket_servo.SetMin(550);
    ticket_servo.SetMax(2485);
    ticket_servo.SetDegree(0);

    //Move the ticket by turning the robot
    move_forward(motor_percent, expected_counts);

    Sleep(1.0);
    ticket_servo.SetDegree(180);
    Sleep(1.0);

    motor_percent = -25;
    turn_left(motor_percent, turn_distance);
    expected_counts = 0.25 * COUNTSPERINCH;
    move_forward(motor_percent, expected_counts);
    turn_distance = 10 * COUNTSPERDEGREE;
    turn_left(motor_percent, turn_distance);

    motor_percent = 25;
    turn_distance = 7.3 * COUNTSPERDEGREE;
    turn_right(motor_percent, turn_distance);

    motor_percent = -25;
    turn_right(motor_percent, turn_distance);

    Sleep(1.0);
    ticket_servo.SetDegree(0);
    motor_percent = 25;
}

void icecream_task(){
    int motor_percent = -50;
    int expected_counts = 17.5 * COUNTSPERINCH;
    int turn_distance = 42.5 * COUNTSPERDEGREE;

    //Turn the robot and move towards the ice cream levers
    turn_right(motor_percent, turn_distance);
    move_forward(motor_percent, expected_counts);

    //Initialize the lever flippign servo
    tray_servo.SetMin(575);
    tray_servo.SetMax(2450);
    tray_servo.SetDegree(160);
    Sleep(0.1);

    //Flip the correct lever
    if(RPS.GetIceCream() == 0) {
        motor_percent = -25;
        LCD.Clear(0xFFfDD0);
        turn_distance = 40 * COUNTSPERDEGREE;
        expected_counts = 4.5 * COUNTSPERINCH;
        turn_right(motor_percent, turn_distance);
        move_forward(motor_percent, expected_counts);
        tray_servo.SetDegree(0);
        Sleep(7.5);
        tray_servo.SetDegree(180);
        Sleep(1.0);
        motor_percent = 25;
        move_forward(motor_percent, expected_counts);
        turn_right(motor_percent, turn_distance);
        expected_counts = 4.5 * COUNTSPERINCH;

        turn_distance = 48.5 * COUNTSPERDEGREE;
        move_forward(motor_percent, expected_counts);
        turn_right(motor_percent, turn_distance);

        //Move back to button
        motor_percent = 85;
        expected_counts = 11 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        // turn_right(motor_percent, turn_distance);
        expected_counts = 16 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_left(motor_percent, turn_distance);

        //Push button
        motor_percent = 95;
        expected_counts = 20 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        

    } else if(RPS.GetIceCream() == 1) {
        motor_percent = -25;
        LCD.SetFontColor(0xFFfDD0);
        LCD.FillRectangle(0, 0, 159, 239);
        LCD.SetFontColor(0x623412);
        LCD.FillRectangle(160, 0, 159, 239);
        LCD.SetFontColor(WHITE);
        expected_counts = 6 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        tray_servo.SetDegree(0);
        Sleep(7.5);
        tray_servo.SetDegree(180);
        Sleep(1.0);
        motor_percent = 25;
        move_forward(motor_percent, expected_counts);
        expected_counts = 4.5 * COUNTSPERINCH;

        turn_distance = 48.5 * COUNTSPERDEGREE;
        move_forward(motor_percent, expected_counts);
        turn_right(motor_percent, turn_distance);

        //Move back to button
        motor_percent = 85;
        expected_counts = 11 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);

        expected_counts = 16 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_left(motor_percent, turn_distance);

        //Push button
        motor_percent = 95;
        expected_counts = 20 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);

    } else if(RPS.GetIceCream() == 2) {
        motor_percent = -25;
        LCD.Clear(0x623412);
        turn_distance = 92 * COUNTSPERDEGREE;
        expected_counts = 3.65 * COUNTSPERINCH;
        motor_percent = 25;
        turn_right(motor_percent, turn_distance);

        motor_percent = -25;
        move_forward(motor_percent, expected_counts);
        turn_right(motor_percent, turn_distance);

        expected_counts = 6.0 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);

        tray_servo.SetDegree(0);
        Sleep(7.5);
        tray_servo.SetDegree(180);
        Sleep(1.0);

        motor_percent = 25;
        move_forward(motor_percent, expected_counts);
        expected_counts = 1.5 * COUNTSPERINCH;

        turn_distance = 48.5 * COUNTSPERDEGREE;
        move_forward(motor_percent, expected_counts);
        turn_right(motor_percent, turn_distance);

        //Move back to button
        motor_percent = 85;
        expected_counts = 11 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);

        expected_counts = 26 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);
        turn_left(motor_percent, turn_distance);

        //Push button
        motor_percent = 100;
        expected_counts = 20 * COUNTSPERINCH;
        move_forward(motor_percent, expected_counts);       
    }
}

int main(void)
{
    RPS.InitializeTouchMenu();
    Sleep(1.0);
    float x, y;
    LCD.WriteAt("TOUCH FOR FINAL ACTION", 2, 111);
    while(!LCD.Touch(&x, &y)){};
    LCD.Clear();
    float time = TimeNow();
     while (TimeNow() - time < 360.0){
    //Constantly reads the CDS value
    float light_value = cds.Value();
    LCD.WriteLine(cds.Value());
    //Begins if light is red
    if (light_value < BLUELIGHT) {
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);

        //Displays the color of the light
        LCD.WriteLine("The color of light is red.");
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);

        //Begins the jukebox task
        while (1) {
            bottom_level();
            burger_task();
            ticket_task();
            icecream_task();
            break;
        }

        //Display that the task has completed
        Sleep(2.0);
        LCD.Clear(BLACK);
        LCD.WriteAt("THE END", 94, 111);
        }
        }

    return 0;
}

