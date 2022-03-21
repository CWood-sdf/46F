#include "BrainOS/Manager.h"
#include "robot-config.h"
void
create_chart(lv_obj_t * parent) {
  lv_obj_t * chart = lv_chart_create(parent, NULL);
  lv_chart_set_type(chart, LV_CHART_TYPE_POINT);
  lv_chart_set_point_count(chart, 460 );
  lv_obj_set_size(chart, 460, 220 );
  lv_obj_set_pos(chart, 10, 10);
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_series_t * s1 = lv_chart_add_series(chart, LV_COLOR_WHITE);
  lv_chart_series_t * s2 = lv_chart_add_series(chart, LV_COLOR_RED);

  int x = 0;
  int t = 0;
  
  while(1) {
    lv_chart_set_next(chart, s1, sin( x / 180.0 * 3.141) * 40 + 50 );
    if(t == 0)
      lv_chart_set_next(chart, s2, cos( x / 180.0 * 3.141) * 20 + 50 );
    if( x++ == 360 )
      x = 0;
    t = 1 - t;
    this_thread::sleep_for(5);
  }
}