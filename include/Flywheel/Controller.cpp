#define NO_MAKE
#include "Flywheel/Controller.h"
const int FlywheelDebugEl::size = sizeof(FlywheelDebugEl) / sizeof(double);
void makeKeyCont(lv_obj_t* key, const char* lbl, lv_color_t color, lv_coord_t distDown) {
  //Create primary container and set padding
  lv_obj_t* keyCont = lv_obj_create(key);
  lv_obj_set_style_pad_all(keyCont, 0, 0);

  //Set container size
  lv_obj_set_width(keyCont, 70);
  lv_obj_set_height(keyCont, 20);

  //Make align and set color and position
  lv_obj_set_align(keyCont, LV_ALIGN_BOTTOM_MID);
  lv_obj_set_style_bg_opa(keyCont, 0, 0);
  lv_obj_set_style_border_opa(keyCont, 0, 0);
  lv_obj_set_pos(keyCont, 0, -distDown);

  //Make colored square
  lv_obj_t* squ = lv_obj_create(keyCont);
  lv_obj_set_size(squ, 15, 15);
  lv_obj_set_style_radius(squ, 03, 0);
  lv_obj_set_style_bg_color(squ, color, 0);
  lv_obj_set_align(squ, LV_ALIGN_LEFT_MID);
  lv_obj_set_style_border_opa(squ, 0, 0);

  //Make label
  lv_obj_t* label2 = lv_label_create(keyCont);
  lv_obj_set_align(label2, LV_ALIGN_LEFT_MID);
  lv_obj_set_pos(label2, 20, 0);
  lv_label_set_text(label2, lbl);
  
}

void basicGraph(bool remake, const char* text, FlywheelDebugEl out){
  static lv_obj_t* chart;
  static lv_chart_series_t* serTarg;
  static lv_chart_series_t* serMeas;
  static lv_chart_series_t* serFilt;
  static lv_chart_series_t* serErr;
  static lv_obj_t* chartLabel;
  static lv_obj_t* key;
  // cout << "LO" << endl;
  if(remake){
    // cout << "LI" << endl;
    // //Make the chart
    chart = lv_chart_create(lv_scr_act());
    //cout << chart << endl;
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 6, 3, 11, 2, true, 40);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -100, 700);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_size(chart, LV_HOR_RES_MAX - 40, LV_VER_RES_MAX - 10);
    lv_chart_set_point_count(chart, 200);
    lv_obj_center(chart);
    lv_obj_set_pos(chart, 20, 0);

    //Make the series
    serTarg = lv_chart_add_series(chart, lv_color_black(), LV_CHART_AXIS_PRIMARY_Y);
    serMeas = lv_chart_add_series(chart, lv_color_make(255, 0, 0), LV_CHART_AXIS_PRIMARY_Y);
    serFilt = lv_chart_add_series(chart, lv_color_make(0, 0, 255), LV_CHART_AXIS_PRIMARY_Y);
    serErr = lv_chart_add_series(chart, lv_color_make(0, 255, 0), LV_CHART_AXIS_PRIMARY_Y);

    //Make the label
    chartLabel = lv_label_create(chart);
    lv_label_set_text(chartLabel, text);

    //Make the key bounding box
    key = lv_obj_create(chart);
    lv_obj_set_align(key, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_bg_color(key, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_pad_all(key, 0, 0);
    lv_obj_set_content_height(key, 100);
    lv_obj_set_content_width(key, 80);
    lv_obj_set_style_bg_opa(key, 0, 0);
    lv_obj_set_style_border_opa(key, 0, 0);

    //Make the key elements
    makeKeyCont(key, "Err", serErr->color, 0);
    makeKeyCont(key, "Targ", serTarg->color, 20);
    makeKeyCont(key, "meas", serMeas->color, 40);
    makeKeyCont(key, "Filt", serFilt->color, 60);
    // cout << remake << endl;
    lv_chart_set_next_value(chart, serTarg, out.targetVel);
    lv_chart_set_next_value(chart, serErr, out.error);
    lv_chart_set_next_value(chart, serMeas, out.measuredVel);
    lv_chart_set_next_value(chart, serFilt, out.filterVel);
  }
  // cout << chart << endl;
  if(chart != NULL){
    lv_chart_set_next_value(chart, serTarg, out.targetVel);
    lv_chart_set_next_value(chart, serErr, out.error);
    lv_chart_set_next_value(chart, serMeas, out.measuredVel);
    lv_chart_set_next_value(chart, serFilt, out.filterVel);
  }
}
void FlywheelTBHEncoder::init() {
  gain = 0.025;
  maxRateGain = 1.5;
  maxRateDrop = 2;
  velCheck = Settled(10, 0.2, 500);
}
void FlywheelTBHEncoder::graph(bool remake) {
  basicGraph(remake, "TBHE ctrl", debug);
}
FlywheelTBHEncoder::FlywheelTBHEncoder(NewMotor<>& m, Encoder p) : mots(m), filter(0.15) {
  init();
  filter.seed(0);
  en = p;
}
FlywheelTBHEncoder::FlywheelTBHEncoder(NewMotor<>& m) : mots(m), filter(0.15) {
  en = Encoder(m[0]);
  init();
  filter.seed(0);
}

void FlywheelTBHEncoder::setTarget(int i) {
  if(velTargets.size() == 0) return;
  target = i;
  if (target < 0) {
    target = 0;
  }
  else if (target >= velTargets.size()) {
    target = velTargets.size() - 1;
  }

  tbh = initialTbh[target];
  if (velTargets.size() != 0) {
    hasTarget = true;
  }
}
void FlywheelTBHEncoder::setTargetSpeed(double t){
  //Loop through the targets and check if t is already in it
  for(int i = 0; i < velTargets.size(); i++){
    if(velTargets[i] == t){
      setTarget(i);
      return;
    }
  }
  //If it isn't, add it
  addTarget(t);
  setTarget(velTargets.size() - 1);
}
void FlywheelTBHEncoder::addTarget(double t) {
  velTargets.push_back(t);
  initialTbh.push_back(t / 6.0);
}

void FlywheelTBHEncoder::step() {
  if (!hasTarget) return;
  static double lastRotation = 0;
  static double lastVel;
  static double lastDesiredVel = 0;
  static double prevErr = 0;
  static int settledCount = 0;
  double velSent = 10;
  bool calcTbh = true;
  double desiredVel = velTargets[target];
  int timeStep = velCheck.timeStep();
  double rotation = en.position(rev);
  double speedEst = abs(rotation - lastRotation) / max((double)timeStep, 1.0) * 60.0 * 1000.0;
  lastRotation = rotation;
  filter.update(speedEst);
  double speed = filter.value();
  double err = desiredVel - speed;
  bool settled = velCheck.settled(err);

  //Leave this empty until TBH is working
  if(settled){
    calcTbh = false;
    settledCount++;
    if(settledCount > 1000/50){
      settledCount = 1000/50;
    }
  }
  else {
    settledCount--;
    if(settledCount > 0){
      calcTbh = false;
    }
  }
  if(desiredVel != lastDesiredVel){
    calcTbh = false;
    settledCount = 0;
  }
  lastDesiredVel = desiredVel;
  if (abs(err) < 10){
    gain = 0.05;
  }
  if (abs(err) < 60) {
    gain = 0.07;
  }
  else {
    gain = 0.02;
  }
  if(calcTbh){
    if(std::signbit(err) != std::signbit(prevErr)){
      tbh = (lastVel + tbh) / 2;
      if(tbh < 0){
        tbh = 0;
      }
      if(tbh > 100){
        tbh = 100;
      }
      initialTbh[target] = tbh;
      velSent = tbh;
    }
    else {
      velSent = lastVel + gain * err;
      
    }
  }
  else {
    velSent = lastVel;
  }
  // cout << velSent << endl;
  // cout << (abs(err) < 200) << ", " << abs(velSent - lastVel) << ", " << (abs(velSent - lastVel) < 4.0) << ", " << !settled << endl;
  if(abs(err) < 200 && abs(err) > 50 && abs(velSent - lastVel) < 1.0 && !settled){
    velSent += -1.0 * (2.0 * std::signbit(err) - 1.0); 
  }
  // cout << signbit(err) << endl;
  // cout << velSent << endl << endl;
  if(velSent - lastVel > maxRateGain){
    velSent = lastVel + maxRateGain;
  }
  else if(velSent - lastVel < -maxRateDrop){
    velSent = lastVel - maxRateDrop;
  }
  if(abs(err) < 60 && velSent - lastVel > 0.1){
    velSent = lastVel + 0.1;
  }
  if(velSent < 0){
    velSent = 0;
  }
  if(velSent > 100){
    velSent = 100;
  }
  debug.set(err, speedEst, speed, desiredVel);
  prevErr = err;
  mots.spinVolt(fwd, velSent);
  lastVel = velSent;
  s(5);
}
