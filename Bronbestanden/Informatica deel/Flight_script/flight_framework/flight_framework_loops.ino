/* Feedback:
Een errordeel dat kijkt wanneer een meetwaarde teveel afwijkt
*/

// compiler haalt dit eruit waardoor het leesbaar is maar geen computing power kost
const int LAUNCH = 0
const int FLIGHT = 1
const int DROP = 2
const int LANDING = 3
const int LANDED = 4


int status = 0;
int currentSpeed = 0;
int maxSpeed = 100;
int height = 0;
int thrust = 0;
int maxThrust = 100;

void PID(){
  Serial.print("PID");
}

void error() {
  ejectParachute();
  stopMotor();
}

void setup() {
}

void loop() {
  // ejects parachute and stops motor when we see something go wrong
  if(errorPushed){
    error();
  }

  while(status == LAUNCH){
    if(errorPushed){
      error();
    }

    thrust += 5;
    PID();
    if(currentSpeed >= maxSpeed){
      status = FLIGHT;
    }
  }

  while(status == FLIGHT){
    if(errorPushed){
      error();
    }
    
    PID();
    if(height == 20){
      status = DROP;
    }
  }

  while(status == DROP){
    if(errorPushed){
      error();
    }

    thrust = 0;
    PID();

    if(height = 10){
      status = LANDING;
    }
  }

  // het landdeel moet verbeterd worden
  while(status == LANDING){
    if(errorPushed){
      error();
    }
    thrust == maxThrust - x;
    if(thrust > 0){
      x += 1;
    }

    if(height == 0){
      status = LANDED;
    }
  }
}
