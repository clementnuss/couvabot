/*
 * Braitenberg basis
 * We have many ways to implement this, we can read IR sensors all the time and
 * adapt speeds accordingly if an obstacle arises - or
 * we can read IR sensors and if sum(sensors) > threshold_A, enter braitenberg until
 * sum(sensors) < threshold_B; with threshold_B < threshold_A.
 */


#define NB_IR_SENSORS 8        // Number of IR sensors
#define IR_MAX_VALUE 4095   // TODO: change this for our sensors
// Usually, IR sensors range from [0; X], X being the closest position for an obstacle
// whereas 0 means that nothing is seen.

// Also requires MAX_SPEED (supposedly 1) and FORWARD_SPEED (maybe we actually use the same as MAX_SPEED?)


// Exemplary function: run braiten providing the variable for left and right wheel_speed
// speeds, these will be updated according to IR sensors.
void braiten(int wheel_speed[2]) {
  // Braitenberg coefficients for left and right wheels if MAX_SPEED == 1
  static double l_weight[NB_IR_SENSORS] = {0.625, 0.1875, 0.1125, 0.05, 0, -0.05, -0.125, -0.1875};
  static double r_weight[NB_IR_SENSORS] = {-0.1875, -0.125, -0.0625, 0, 0.0625, 0.125, 0.1875, 0.625};

  // Distance sensors
  static double ds_value[NB_IR_SENSORS];

  // Get distance sensors
  int i;
  for (i = 0; i < NB_IR_SENSORS; i++)
    ds_value[i] = get_distance_sensor(sensor[i]); // range: ? to test - change function of course

  // Bias speeds (when sensors do not detect anything)
  // Two options - pick one
  /*  1. use FORWARD_SPEED as the bias speed
  double left_speed  = FORWARD_SPEED;
  double right_speed = FORWARD_SPEED;
  //*/
  //* 2. use given speeds to add braitenberg on top of it
  double left_speed  = (double) wheel_speed[0];
  double right_speed = (double) wheel_speed[1];
  //*

  // Modify speed according to sensory feedback
  for (i = 0; i < NB_IR_SENSORS; i++)
  {
    left_speed  += l_weight[i]*((1.0-ds_value[i]/4095.0));
    right_speed += r_weight[i]*((1.0-ds_value[i]/4095.0));
  }

  // Bound speeds
  double reduction_factor;
  if (right_speed > MAX_SPEED || left_speed > MAX_SPEED)
  {
    // Determine biggest reduction factor
    if (right_speed > left_speed)
      reduction_factor = right_speed / (double) MAX_SPEED;
    else
      reduction_factor = left_speed  / (double) MAX_SPEED;

    // Divide speeds by that factor so that highest speed is now equal to MAX_SPEED
    left_speed  /= reduction_factor;
    right_speed /= reduction_factor;
  }

  // Update wheel speeds
  wheel_speed[0] = (int) floor(left_speed);
  wheel_speed[1] = (int) floor(right_speed);


  /* Set speed - braiten function could set speed by itself, dunno what is best.
  set_speed(left_speed, right_speed);     // Change function's name...
  //*/


  return 0;
}
