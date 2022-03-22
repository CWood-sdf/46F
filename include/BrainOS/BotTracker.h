#include "BrainOS/Manager.h"
#include "robot-config.h"
#include "v5lvgl.h"
void
drawBotPos() {
  static lv_obj_t* graph = lv_chart_create(lv_scr_act(), NULL);
  lv_chart_set_type(graph, LV_CHART_TYPE_AREA | LV_CHART_TYPE_NONE);
  lv_obj_set_size(graph, 220, 220);
  lv_obj_set_pos(graph, 10, 10);
  
  while(1) {
    this_thread::sleep_for(5);
  }
}