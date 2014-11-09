/** Copyright (C) 2014 Harijs Grinbergs
***
*** This file is a part of the ENIGMA Development Environment.
***
*** ENIGMA is free software: you can redistribute it and/or modify it under the
*** terms of the GNU General Public License as published by the Free Software
*** Foundation, version 3 of the license or any later version.
***
*** This application and its source code is distributed AS-IS, WITHOUT ANY
*** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*** FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
*** details.
***
*** You should have received a copy of the GNU General Public License along
*** with this code. If not, see <http://www.gnu.org/licenses/>
**/

#include <unordered_map>
#include <string>
using std::string;
using std::unordered_map;
using std::pair;

#include "Universal_System/var4.h"
#include "Universal_System/CallbackArrays.h" //For mouse_check_button
#include "Universal_System/resource_data.h" //For script_execute
//#include "Universal_System/spritestruct.h"
#include "Graphics_Systems/General/GSsprite.h"
#include "Graphics_Systems/General/GSfont.h"
#include "Graphics_Systems/General/GScolors.h"

#include "skins.h"
#include "sliders.h"
#include "include.h"
#include "common.h"

namespace gui
{
  unordered_map<unsigned int, gui_slider> gui_sliders;
	unsigned int gui_sliders_maxid = 0;

	extern int gui_bound_skin;
	extern unordered_map<unsigned int, gui_skin> gui_skins;
	extern unsigned int gui_skins_maxid;

	extern bool windowStopPropagation; //This stops event propagation between window elements

	//Implements slider class
	void gui_slider::reset(){
		text = "", state = 0, sprite = sprite_hover = sprite_active = sprite_on = sprite_on_hover = sprite_on_active = -1;
		active = false;
		callback = -1;
		visible = true;
		font_styles[0].halign = font_styles[1].halign = font_styles[2].halign = font_styles[3].halign = font_styles[4].halign = font_styles[5].halign = enigma_user::fa_left;
		font_styles[0].valign = font_styles[1].valign = font_styles[2].valign = font_styles[3].valign = font_styles[4].valign = font_styles[5].valign = enigma_user::fa_middle;
	}

	gui_slider::gui_slider(){
	  parent_id = -1;
		reset();
	}

	//Update all possible slider states (hover, click etc.)
	void gui_slider::update(gs_scalar ox, gs_scalar oy, gs_scalar tx, gs_scalar ty){
		if (box.point_inside(tx-ox,ty-oy)){
      gui::windowStopPropagation = true;
			if (enigma_user::mouse_check_button_pressed(enigma_user::mb_left)){
        if (active == false){
          state = enigma_user::gui_state_active;
        }else{
          state = enigma_user::gui_state_on_active;
        }
			}else{
				if (state != enigma_user::gui_state_active &&  state != enigma_user::gui_state_on_active){
					if (active == false){
						state = enigma_user::gui_state_hover;
					}else{
						state = enigma_user::gui_state_on_hover;
					}
				}else{
					if (enigma_user::mouse_check_button_released(enigma_user::mb_left)){
						active = !active;
						if (callback != -1){
							enigma_user::script_execute(callback, id, active);
						}

						if (active == false){
							state = enigma_user::gui_state_hover;
						}else{
							state = enigma_user::gui_state_on_hover;
						}
					}
				}
			}
		}else{
			if (active == false){
				state = enigma_user::gui_state_default;
			}else{
				state = enigma_user::gui_state_on;
			}
		}
	}

	void gui_slider::draw(gs_scalar ox, gs_scalar oy){
		//Draw button
		switch (state){
			case enigma_user::gui_state_default: //Default off
				if (sprite!=-1){
					enigma_user::draw_sprite_padded(sprite,-1,border.left,border.top,border.right,border.bottom,ox + box.x,oy + box.y,ox + box.x+box.w,oy + box.y+box.h);
				}
			break;
			case enigma_user::gui_state_hover: //Default off hover
				if (sprite_hover!=-1){
					enigma_user::draw_sprite_padded(sprite_hover,-1,border.left,border.top,border.right,border.bottom,ox + box.x,oy + box.y,ox + box.x+box.w,oy + box.y+box.h);
				}
			break;
			case enigma_user::gui_state_active: //Default active
				if (sprite_active!=-1){
					enigma_user::draw_sprite_padded(sprite_active,-1,border.left,border.top,border.right,border.bottom,ox + box.x,oy + box.y,ox + box.x+box.w,oy + box.y+box.h);
				}
			break;
			case enigma_user::gui_state_on: //Default on
				if (sprite_on!=-1){
					enigma_user::draw_sprite_padded(sprite_on,-1,border.left,border.top,border.right,border.bottom,ox + box.x,oy + box.y,ox + box.x+box.w,oy + box.y+box.h);
				}
			break;
			case enigma_user::gui_state_on_hover: //Default on hover
				if (sprite_on_hover!=-1){
					enigma_user::draw_sprite_padded(sprite_on_hover,-1,border.left,border.top,border.right,border.bottom,ox + box.x,oy + box.y,ox + box.x+box.w,oy + box.y+box.h);
				}
			break;
      case enigma_user::gui_state_on_active: //Default on active
				if (sprite_on_active!=-1){
					enigma_user::draw_sprite_padded(sprite_on_active,-1,border.left,border.top,border.right,border.bottom,ox + box.x,oy + box.y,ox + box.x+box.w,oy + box.y+box.h);
				}
			break;
		}

		//Draw text
		font_styles[state].use();
		enigma_user::draw_text(ox + font_styles[state].textx,oy + font_styles[state].texty,text);
	}

	void gui_slider::update_text_pos(int state){
		if (state == -1){
			update_text_pos(enigma_user::gui_state_default);
			update_text_pos(enigma_user::gui_state_hover);
			update_text_pos(enigma_user::gui_state_active);
			update_text_pos(enigma_user::gui_state_on);
			update_text_pos(enigma_user::gui_state_on_hover);
			update_text_pos(enigma_user::gui_state_on_active);
		}

		font_style* style = &font_styles[state];

		if (style->halign == enigma_user::fa_left){
			style->textx = box.x+padding.left;
		}else if (style->halign == enigma_user::fa_center){
			style->textx = box.x+box.w/2.0;
		}else if (style->halign == enigma_user::fa_right){
			style->textx = box.x+box.w-padding.right;
		}

		if (style->valign == enigma_user::fa_top){
			style->texty = box.y+padding.top;
		}else if (style->valign == enigma_user::fa_middle){
			style->texty = box.y+box.h/2.0;
		}else if (style->valign == enigma_user::fa_bottom){
			style->texty = box.y+box.h-padding.bottom;
		}
	}
}

namespace enigma_user
{
	int gui_slider_create(){
		if (gui::gui_bound_skin == -1){ //Add default one
			gui::gui_sliders.insert(pair<unsigned int, gui::gui_slider >(gui::gui_sliders_maxid, gui::gui_slider()));
		}else{
			gui::gui_sliders.insert(pair<unsigned int, gui::gui_slider >(gui::gui_sliders_maxid, gui::gui_sliders[gui::gui_skins[gui::gui_bound_skin].slider_style]));
		}
		gui::gui_sliders[gui::gui_sliders_maxid].visible = true;
		gui::gui_sliders[gui::gui_sliders_maxid].id = gui::gui_sliders_maxid;
		return (gui::gui_sliders_maxid++);
	}

	int gui_slider_create(gs_scalar x, gs_scalar y, gs_scalar w, gs_scalar h, string text){
		if (gui::gui_bound_skin == -1){ //Add default one
			gui::gui_sliders.insert(pair<unsigned int, gui::gui_slider >(gui::gui_sliders_maxid, gui::gui_slider()));
		}else{
			gui::gui_sliders.insert(pair<unsigned int, gui::gui_slider >(gui::gui_sliders_maxid, gui::gui_sliders[gui::gui_skins[gui::gui_bound_skin].slider_style]));
		}
		gui::gui_sliders[gui::gui_sliders_maxid].visible = true;
		gui::gui_sliders[gui::gui_sliders_maxid].id = gui::gui_sliders_maxid;
		gui::gui_sliders[gui::gui_sliders_maxid].box.x = x;
		gui::gui_sliders[gui::gui_sliders_maxid].box.y = y;
		gui::gui_sliders[gui::gui_sliders_maxid].box.w = w;
		gui::gui_sliders[gui::gui_sliders_maxid].box.h = h;
		gui::gui_sliders[gui::gui_sliders_maxid].text = text;
		gui::gui_sliders[gui::gui_sliders_maxid].update_text_pos();
		return (gui::gui_sliders_maxid++);
	}

	void gui_slider_destroy(int id){
		gui::gui_sliders.erase(gui::gui_sliders.find(id));
	}

	void gui_slider_set_text(int id, string text){
		gui::gui_sliders[id].text = text;
	}

	void gui_slider_set_position(int id, gs_scalar x, gs_scalar y){
		gui::gui_sliders[id].box.x = x;
		gui::gui_sliders[id].box.y = y;
	}

	void gui_slider_set_font(int id, int state, int font){
		if (state == enigma_user::gui_state_all){
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_default].font = font;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_hover].font = font;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_active].font = font;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on].font = font;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_hover].font = font;
      gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_active].font = font;
		}else{
			gui::gui_sliders[id].font_styles[state].font = font;
		}
	}

	void gui_slider_set_font_halign(int id, int state, unsigned int halign){
		if (state == enigma_user::gui_state_all){
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_default].halign = halign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_hover].halign = halign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_active].halign = halign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on].halign = halign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_hover].halign = halign;
      gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_active].halign = halign;
			gui::gui_sliders[id].update_text_pos();
		}else{
			gui::gui_sliders[id].font_styles[state].halign = halign;
			gui::gui_sliders[id].update_text_pos(state);
		}
	}

	void gui_slider_set_font_valign(int id, int state, unsigned int valign){
		if (state == enigma_user::gui_state_all){
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_default].valign = valign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_hover].valign = valign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_active].valign = valign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on].valign = valign;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_hover].valign = valign;
      gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_active].valign = valign;
			gui::gui_sliders[id].update_text_pos();
		}else{
			gui::gui_sliders[id].font_styles[state].valign = valign;
			gui::gui_sliders[id].update_text_pos(state);
		}
	}

	void gui_slider_set_font_color(int id, int state, int color){
		if (state == enigma_user::gui_state_all){
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_default].color = color;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_hover].color = color;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_active].color = color;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on].color = color;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_hover].color = color;
      gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_active].color = color;
		}else{
			gui::gui_sliders[id].font_styles[state].color = color;
		}
	}

	void gui_slider_set_font_alpha(int id, int state, gs_scalar alpha){
		if (state == enigma_user::gui_state_all){
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_default].alpha = alpha;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_hover].alpha = alpha;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_active].alpha = alpha;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on].alpha = alpha;
			gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_hover].alpha = alpha;
      gui::gui_sliders[id].font_styles[enigma_user::gui_state_on_active].alpha = alpha;
		}else{
			gui::gui_sliders[id].font_styles[state].alpha = alpha;
		}
	}

	void gui_slider_set_sprite(int id, int state, int sprid){
		switch (state){
			case enigma_user::gui_state_default: gui::gui_sliders[id].sprite = sprid; break;
			case enigma_user::gui_state_hover: gui::gui_sliders[id].sprite_hover = sprid; break;
			case enigma_user::gui_state_active: gui::gui_sliders[id].sprite_active = sprid; break;
			case enigma_user::gui_state_on: gui::gui_sliders[id].sprite_on = sprid; break;
			case enigma_user::gui_state_on_hover: gui::gui_sliders[id].sprite_on_hover = sprid; break;
			case enigma_user::gui_state_on_active: gui::gui_sliders[id].sprite_on_active = sprid; break;
		}
	}

	void gui_slider_set_size(int id, gs_scalar w, gs_scalar h){
		gui::gui_sliders[id].box.w = w;
		gui::gui_sliders[id].box.h = h;
		gui::gui_sliders[id].update_text_pos();
	}

	void gui_slider_set_padding(int id, gs_scalar left, gs_scalar top, gs_scalar right, gs_scalar bottom){
		gui::gui_sliders[id].padding.set(left,top,right,bottom);
		gui::gui_sliders[id].update_text_pos();
	}

	void gui_slider_set_border(int id, gs_scalar left, gs_scalar top, gs_scalar right, gs_scalar bottom){
		gui::gui_sliders[id].border.set(left,top,right,bottom);
	}

	void gui_slider_set_callback(int id, int script_id){
		gui::gui_sliders[id].callback = script_id;
	}

	int gui_slider_get_state(int id){
		return gui::gui_sliders[id].state;
	}

	bool gui_slider_get_active(int id){
		return gui::gui_sliders[id].active;
	}

	void gui_slider_set_visible(int id, bool visible){
		gui::gui_sliders[id].visible = visible;
	}

	void gui_slider_draw(int id){
		unsigned int phalign = enigma_user::draw_get_halign();
		unsigned int pvalign = enigma_user::draw_get_valign();
		int pcolor = enigma_user::draw_get_color();
		gs_scalar palpha = enigma_user::draw_get_alpha();
    gui::gui_sliders[id].update();
		gui::gui_sliders[id].draw();
		enigma_user::draw_set_halign(phalign);
		enigma_user::draw_set_valign(pvalign);
		enigma_user::draw_set_color(pcolor);
		enigma_user::draw_set_alpha(palpha);
	}

	void gui_sliders_draw(){
		unsigned int phalign = enigma_user::draw_get_halign();
		unsigned int pvalign = enigma_user::draw_get_valign();
		int pcolor = enigma_user::draw_get_color();
		gs_scalar palpha = enigma_user::draw_get_alpha();
		for (unsigned int i=0; i<gui::gui_sliders_maxid; ++i){
			if (gui::gui_sliders[i].visible == true && gui::gui_sliders[i].parent_id == -1){
      	gui::gui_sliders[i].update();
				gui::gui_sliders[i].draw();
			}
		}
		enigma_user::draw_set_halign(phalign);
		enigma_user::draw_set_valign(pvalign);
		enigma_user::draw_set_color(pcolor);
		enigma_user::draw_set_alpha(palpha);
	}
}
