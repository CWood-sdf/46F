#define NO_MAKE
#include "Flywheel/Controller.h"

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
  if(remake){
    //Make the chart
    lv_obj_t* chart = lv_obj_create(lv_scr_act());
    lv_chart_set_point_count(chart, 200);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -100, 1000);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_size(chart, BRAIN_WIDTH - 10, BRAIN_HEIGHT - 10);
    lv_obj_center(chart);

    //Make the series
    lv_chart_series_t* serTarg = lv_chart_add_series(chart, lv_color_black(), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t* serMeas = lv_chart_add_series(chart, lv_color_make(255, 0, 0), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t* serFilt = lv_chart_add_series(chart, lv_color_make(0, 0, 255), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t* serErr = lv_chart_add_series(chart, lv_color_make(0, 255, 0), LV_CHART_AXIS_PRIMARY_Y);

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
  }
  lv_chart_set_next_value(chart, serTarg, out.targetVel);
  lv_chart_set_next_value(chart, serErr, out.error);
  lv_chart_set_next_value(chart, serMeas, out.measuredVel);
  lv_chart_set_next_value(chart, serFilt, out.filterVel);
}
void FlywheelTBH::graph(bool remake){
  basicGraph(remake, "TBH ctrl", debug);
}
FlywheelTBH::FlywheelTBH(NewMotor<> m, vex::triport::port& p) : en(p), filter(0.7){
  mots = m;
  filter.seed(0);
}
void FlywheelTBH::setTarget(int i){
  target = i;
  if(target < 0) 
    target = 0;
  else if(target >= velTargets.size()) 
    target = velTargets.size() - 1;
  tbh = initialTbh[target];
}
void FlywheelTBH::addTarget(double t){
  velTargets.push_back(t);
  initialTbh.push_back(0);
}
void FlywheelTBH::step(){
  static double lastRotation = 0;
  static int lastVel;
  static double lastDesiredVel = 0;
  static double prevErr = 0;
  int velSent = 10;
  bool calcTbh = true;
  double desiredVel = velTargets[target];
  int timeStep = velCheck.timeStep();
  double rotation = en.rotation(rotationUnits::rev);
  double speedEst = (rotation - lastRotation) / (double)timeStep * 60.0;
  lastRotation = rotation;
  filter.update(speedEst);
  double speed = filter.value();
  double err = speed - desiredVel;
  bool settled = velCheck.settled(err);
  if(settled){
    calcTbh = false;
  }
  if(desiredVel != lastDesiredVel){
    calcTbh = false;
  }
  lastDesiredVel = desiredVel;
  if(calcTbh){
    if(signbit(err) != signbit(prevErr)){
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
  if(velSent < 0){
    velSent = 0;
  }
  if(velSent > 100){
    velSent = 100;
  }
  debug.set(err, speedEst, speed, desiredVel);
  prevErr = err;
  mots.spin(fwd, velSent);
  lastVel = velSent;
}


FlywheelPID::FlywheelPID(NewMotor<> m, vex::triport::port& p) : en(p), filter(0.7){
  mots = m;
  filter.seed(0);
}
void FlywheelPID::setTarget(int i){
  target = i;
  if(target < 0) 
    target = 0;
  else if(target >= velTargets.size()) 
    target = velTargets.size() - 1;
  ctrl.setTarget(velTargets[i]);
}
void FlywheelPID::step(){
  static double lastRotation = 0;
  double desiredVel = velTargets[target];
  int timeStep = velCheck.timeStep();
  double rotation = en.rotation(rotationUnits::rev);
  double speedEst = (rotation - lastRotation) / (double)timeStep * 60.0;
  lastRotation = rotation;
  filter.update(speedEst);
  double speed = filter.value();
  double err = speed - desiredVel;
  bool settled = velCheck.settled(err);
  debug.set(err, speedEst, speed, desiredVel);
  mots.spin(fwd, ctrl.getVal(err));
}

void FlywheelPID::graph(bool remake){
  basicGraph(remake, "PID ctrl", debug);
}