#include "pch.hpp"
#include "anim-io.hpp"
#include "anim.hpp"
#include "frame.hpp"
#include "game/util/store.hpp"
#include "game/util/resource-helper.hpp"
#include "game/core/entities.hpp"
#include "game/core/sprites.hpp"
#include "game/core/anims.hpp"
#include "game/core/common.hpp"
#include "game/entity/util/hitbox.hpp"
#include "engine/graphic/sprite/sprite.hpp"

inline void save_hitbox(cp<Anim> anim, nlohmann::json& root) {
  auto hitbox_source = anim->get_hitbox_source();
  return_if (!hitbox_source);
  return_if (!scast<bool>(*hitbox_source));

  rauto hitbox_node = make_node(root, "hitbox");

  // сохранить полигоны хитбокса
  rauto polygons_node = make_node(hitbox_node, "polygons");
  for (uint poly_idx = 0; crauto polygon: hitbox_source->polygons) {
    cont_if( !polygon);

    rauto cur_poly_node = make_node(polygons_node, "poly_" + n2s(poly_idx));
    if (polygon.offset.not_zero())
      cur_poly_node["offset"] = Vector<real>{polygon.offset.x, polygon.offset.y};

    // сохранить точки полигона
    if ( !polygon.points.empty()) {
      rauto points_node = make_node(cur_poly_node, "points");

      for (uint point_idx = 0; crauto point: polygon.points) {
        points_node["P" + n2s(point_idx)] = Vector<real>{point.x, point.y};
        ++point_idx;
      } // fpr points
    }

    ++poly_idx;
  } // for polygons
} // save_hitbox

inline void load_hitbox(Anim& anim, cr<nlohmann::json> hitbox_node) {
  return_if(hitbox_node.empty());
  assert(hpw::entity_mgr);
  auto hitbox_source = hpw::entity_mgr->get_hitbox_pool().new_object<Hitbox>();

  // загрузить полигоны хитбокса
  auto polygons_node = hitbox_node["polygons"];
  for (crauto poly_name: root_tags(polygons_node)) {
    Polygon loaded_poly;

    auto poly_node = polygons_node[poly_name];
    auto poly_offset_v = poly_node.value<Vector<real>>("offset", Vector<real>{0, 0});
    loaded_poly.offset.x = poly_offset_v.at(0);
    loaded_poly.offset.y = poly_offset_v.at(1);

    // загрузить точки полигона
    auto points_node = poly_node["points"];
    for (crauto point_name: root_tags(points_node)) {
      auto point_v = points_node.value<Vector<real>>(point_name, Vector<real>{0, 0});
      loaded_poly.points.emplace_back( Vec(
        point_v.at(0),
        point_v.at(1)
      ) );
    } // fpr points

    hitbox_source->polygons.emplace_back(loaded_poly);
  } // for polygons

  if (scast<bool>(*hitbox_source))
    anim.update_hitbox(hitbox_source);
} // save_hitbox

void read_anims(cr<nlohmann::json> src) {
  log_debug << "read all anims...";

  // прочитать все анимации
  auto animations_node = src["animations"];
  auto anim_names = root_tags(animations_node);
  std::sort(anim_names.begin(), anim_names.end());

  cfor (i, anim_names.size()) {
    crauto anim_name {anim_names[i]};
    cauto anim_node = animations_node[anim_name];
    cauto anim = read_anim(anim_node);
    hpw::anim_mgr->add_anim(anim_name, anim);
  } // root tags
} // read_anims

Shared<Anim> read_anim(cr<nlohmann::json> anim_node) {
  // анимация будет сохранена в Anim_mgr
  auto anim = new_shared<Anim>();

  // загрузка хитбокса
  if (anim_node.contains("hitbox"))
    load_hitbox(*anim, anim_node["hitbox"]);

  // нода с кадрами
  if (anim_node.contains("frames")) {
    cauto frames_node = anim_node["frames"];

    // прочитать все кадры
    for (crauto frame_name: root_tags(frames_node)) {
      auto frame = new_shared<Frame>();
      // нода этого кадра
      auto cur_frame_node = frames_node[frame_name];
      // прочитать длительность кадра
      frame->duration = cur_frame_node.value("duration", real{0});
      // прочитать число разворотов
      frame->source_ctx.max_directions = cur_frame_node.value("directions", int{0});
      // прочитать коррекцию артефактов
      auto ccf_name = cur_frame_node.value("ccf", Str{});
      if (!ccf_name.empty())
        frame->source_ctx.ccf = convert_to_ccf(ccf_name);
      auto cgp_name = cur_frame_node.value("cgp", Str{});
      if (!cgp_name.empty())
        frame->source_ctx.cgp = convert_to_cgp(cgp_name);
      auto rotate_offset_v = cur_frame_node.value("rotate offset", Vector<real>{});
      if (!rotate_offset_v.empty()) {
        frame->source_ctx.rotate_offset.x = rotate_offset_v.at(0);
        frame->source_ctx.rotate_offset.y = rotate_offset_v.at(1);
      }
      // прочитать источник кадра
      auto sprite_path = cur_frame_node.value("sprite path", Str{});
      if (!sprite_path.empty()) {
        auto finded_sprite = hpw::sprites.find(sprite_path);
        if (finded_sprite)
          frame->source_ctx.direct_0.sprite = finded_sprite;
      }
      // смещение отрисовки относительно центра спрайта
      auto sprite_offset_v = cur_frame_node.value("sprite offset", Vector<real>{});
      if (!sprite_offset_v.empty()) {
        frame->source_ctx.direct_0.offset.x = sprite_offset_v.at(0);
        frame->source_ctx.direct_0.offset.y = sprite_offset_v.at(1);
      }

      frame->reinit_directions_by_source();
      anim->add_frame(frame);
    } // for frames_node tags
  } // if frames in json

  return anim;
} // read_anim

void save_anims(nlohmann::json& dst, cr<Str> save_path) {
  assert (hpw::anim_mgr);
  log_info << "save all anims...";
  dst.clear();

  // нода с анимациями
  auto anims = hpw::anim_mgr->get_anims();
  return_if (anims.empty());
  rauto animations_node = make_node(dst, "animations");
  for (crauto anim: anims) {
    cont_if(!anim);
    // нода с именем анимации
    rauto cur_anim_node = make_node(animations_node, anim->get_name());
    save_hitbox(anim.get(), cur_anim_node);

    // нода с кадрами
    auto frames = anim->get_frames();
    cont_if(frames.empty());
    rauto frames_node = make_node(cur_anim_node, "frames");
    for (crauto frame: frames) {
      cont_if(!frame);
      // нода по имени (uid) кадра
      rauto cur_frame_node = make_node(frames_node, frame->get_name());
      // длительность кадра
      if (frame->duration > 0)
        cur_frame_node["duration"] = frame->duration;
      // сколько разворотов
      if (frame->source_ctx.max_directions > 0)
        cur_frame_node["directions"] = frame->source_ctx.max_directions;
      // режимы устранения артефактов
      if (frame->source_ctx.ccf != Color_compute{})
        cur_frame_node["ccf"] = convert(frame->source_ctx.ccf);
      if (frame->source_ctx.cgp != Color_get_pattern{})
        cur_frame_node["cgp"] = convert(frame->source_ctx.cgp);
      // для точной подгонки артефактов при повороте
      if (frame->source_ctx.rotate_offset.not_zero()) {
        cur_frame_node["rotate offset"] = Vector<real>{
          frame->source_ctx.rotate_offset.x,
          frame->source_ctx.rotate_offset.y
        };
      } // if rotate_offset

      if (auto sprite = frame->source_ctx.direct_0.sprite; !sprite.expired()) {
        // путь к файлу с кадром
        if (auto sprite_lock = sprite.lock(); !sprite_lock->get_path().empty()) {
          cauto sprite_path = sprite_lock->get_path();
          cur_frame_node["sprite path"] = sprite_path;
        }
        // смещение отрисовки относительно центра спрайта
        if (frame->source_ctx.direct_0.offset.not_zero()) {
          cur_frame_node["sprite offset"] = Vector<real>{
            frame->source_ctx.direct_0.offset.x,
            frame->source_ctx.direct_0.offset.y
          };
        }
        // TODO hitbox save
      } // if frame->source_ctx.direct_0.sprite
    } // for frames
  } // for anims

  save(dst, save_path);
} // save_anims

nlohmann::json get_anim_config() {
  return json_from_res("scripts/gameplay/animation.json"); 
}
