#include "BrainOS/Manager.h"
#include "robot-config.h"
#include "lvgl_inc.h"
std::vector<lv_point_t> pts = { {0, 0}, {100, 100}, {50, 100}, {150, 150} };
std::vector<lv_point_t> actualPts;
struct Node {
    lv_obj_t* val;
    Node* next = NULL;
    Node(lv_obj_t* v) {
        val = v;
    }
    ~Node() {
        lv_mem_free(val);
        if (next != NULL) {
            delete next;
        }
    }
};

void animCallback(void* a, int32_t param) {
    Node* cont = (Node*)a;
    Node* line = cont->next;
    PVector pos = wc.botPos();
    double angle = wc.botAngle();

    short height = LV_VER_RES;

    //Zero position
    /*pos.x += 72;
    pos.y += 72;*/

    pos *= height / 144.0;

    lv_obj_t* containerObj = cont->val;
    lv_obj_t* lineObj = line->val;
    double width = 10;
    double halfContWidth = lv_obj_get_width(containerObj) / 2 - 3;
    std::vector<PVector> lineCorners = {
        {-width, width},
        {-width, -width},
        {width, -width},
        {width, width}
    };
    actualPts.clear();
    for (PVector& v : lineCorners) {
        v.rotate(angle);
        actualPts.push_back(v + PVector(halfContWidth, halfContWidth));
    }
    lv_line_set_points(lineObj, actualPts.data(), actualPts.size());
    
    lv_obj_align(containerObj, LV_ALIGN_CENTER, pos.x, pos.y);
    lv_obj_set_size(containerObj, width * M_SQRT2 + 30, width * M_SQRT2 + 30);
}

lv_obj_t* glblObj;
void cool() {

    short gridHeight = LV_VER_RES;
    short sixthHeight = (LV_VER_RES / 6);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(150, 150, 150), 0);
    lv_obj_t* gridContainer = lv_obj_create(lv_scr_act());
    lv_obj_set_style_pad_all(lv_scr_act(), 0, 0);
    lv_obj_set_style_bg_opa(gridContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(gridContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_size(gridContainer, gridHeight + 1, gridHeight + 1);
    lv_obj_set_style_pad_all(gridContainer, -2, 0);
    lv_obj_set_pos(gridContainer, 0, 0);
    
    pts.clear();
    for (int i = 1; i < 6; i++) {   
        if (i % 2 == 0) {
            pts.push_back({ (short)(sixthHeight * (i)), (short)0 });
            pts.push_back({ (short)(sixthHeight * (i)), gridHeight });
        }
        else {
            pts.push_back({ (short)(sixthHeight * (i)), gridHeight });
            pts.push_back({ (short)(sixthHeight * (i)), (short)0 });
        }
    }
    pts.push_back({ gridHeight, (short)0 });
    for (int i = 1; i < 6; i++) {
        if (i % 2 == 0) {
            pts.push_back({ (short)0, (short)(sixthHeight * (i)) });
            pts.push_back({ gridHeight, (short)(sixthHeight * (i)) });
        }
        else {
            pts.push_back({ gridHeight, (short)(sixthHeight * (i)) });
            pts.push_back({ (short)0, (short)(sixthHeight * (i)) });
        }
    }
    pts.push_back({ (short)0, (short)0 });
    pts.push_back({ gridHeight, (short)0 });
    pts.push_back({ gridHeight, gridHeight });
    pts.push_back({ (short)0, gridHeight });
    
    lv_obj_t* line = lv_line_create(gridContainer);
    lv_line_set_points(line, pts.data(), pts.size());
    //lv_obj_add_style(line, &style_title, 0);
    lv_obj_set_style_line_width(line, 1, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    {
        auto basicAlign = [](lv_obj_t* obj, int y) {
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 260, y);
        };
        lv_obj_t* labelX;
        labelX = lv_label_create(lv_scr_act());
        basicAlign(labelX, 5);
        std::string setTextX = "x: ";
        setTextX += "3";
        lv_label_set_text(labelX, setTextX.data());

        lv_obj_t* labelY;
        labelY = lv_label_create(lv_scr_act());
        basicAlign(labelY, 25);
        std::string setTextY = "y: ";
        setTextY += "3.1415936539";
        lv_label_set_text(labelY, setTextY.data());

        lv_obj_t* labelA;
        labelA = lv_label_create(lv_scr_act());
        std::string setTextA = "a: ";
        setTextA += "3";
        lv_label_set_text(labelA, setTextA.data());
        basicAlign(labelA, 45);

        lv_obj_t* labelV;
        labelV = lv_label_create(lv_scr_act());
        std::string setTextV = "v: ";
        setTextV += (char)(lv_version_major() + 48);
        setTextV += ".";
        setTextV += (char)(lv_version_minor() + 48);
        setTextV += ".";
        setTextV += (char)(lv_version_patch() + 48);
        lv_label_set_text(labelV, setTextV.data());
        basicAlign(labelV, 65);
        
    }
    
    std::vector<lv_obj_t*> squs;
    for (int i = 0; i < 4; i++) {
        lv_obj_t* blueSqu = lv_obj_create(gridContainer);
        lv_obj_align(blueSqu, LV_ALIGN_TOP_LEFT, i * sixthHeight, i * sixthHeight);
        lv_obj_set_size(blueSqu, sixthHeight, sixthHeight);
        lv_obj_set_style_bg_color(blueSqu, lv_color_make(255, 0, 0), 0);
        lv_obj_set_style_border_width(blueSqu, 0, 0);
        lv_obj_set_style_border_color(blueSqu, lv_color_make(0, 0, 0), 0);
        lv_obj_set_style_radius(blueSqu, 0, 0);
        //Necessary for later deallocation
        squs.push_back(blueSqu);
    }
    lv_obj_t* botContainer = lv_obj_create(gridContainer);
    lv_obj_t* line2 = lv_line_create(botContainer);
    lv_obj_t* centerPt = lv_obj_create(botContainer);

    Node* cont = new Node(botContainer);
    Node* l = new Node(line2);
    Node* c = new Node(centerPt);
    cont->next = l;
    l->next = c;

    lv_obj_set_style_line_width(line2, 3, 0);
    lv_obj_set_style_line_rounded(line2, true, 0);
    lv_obj_set_style_line_color(line2, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_line_opa(line2, LV_OPA_MAX, 0);

    lv_obj_set_style_bg_opa(botContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(botContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(botContainer, 0, 0);

    lv_obj_set_align(centerPt, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(centerPt, 5, 0);
    lv_obj_set_size(centerPt, 5, 5);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_exec_cb(&anim, animCallback);
    lv_anim_set_var(&anim, cont);
    lv_anim_set_time(&anim, 100);
    lv_anim_set_values(&anim, 0, 100);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim);
    /*lv_obj_t* label;
    lv_obj_t* btnRight = lv_btn_create(lv_scr_act());
    lv_obj_align(btnRight, LV_ALIGN_BOTTOM_RIGHT, -50, -50);
    label = lv_label_create(btnRight);
    lv_label_set_text(label, ">");*/
}
