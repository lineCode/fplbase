// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "precompiled.h"
#include "fplbase/material.h"
#include "fplbase/renderer.h"
#include "fplbase/utilities.h"
#include "mathfu/glsl_mappings.h"

namespace fpl {

using mathfu::vec2;
using mathfu::vec2i;
using mathfu::vec3;
using mathfu::vec3i;
using mathfu::vec4;
using mathfu::vec4i;
using mathfu::mat4;

void Texture::Load() {
  data_ =
      renderer_->LoadAndUnpackTexture(filename_.c_str(), &size_, &has_alpha_);
  original_size_ = size_;
  if (!data_) {
    LogError(kApplication, "texture load: %s: %s",
             filename_.c_str(), renderer_->last_error().c_str());
  }
}

void Texture::LoadFromMemory(const uint8_t *data, const vec2i &size,
                             TextureFormat format, bool has_alpha,
                             bool mipmaps) {
  size_ = size;
  original_size_ = size_;
  has_alpha_ = has_alpha;
  desired_ = format;
  mipmaps_ = mipmaps;
  id_ = renderer_->CreateTexture(data, size_, has_alpha_, mipmaps_, desired_);
}

void Texture::Finalize() {
  if (data_) {
    id_ = renderer_->CreateTexture(data_, size_, has_alpha_, mipmaps_, desired_);
    free(data_);
    data_ = nullptr;
  }
}

void Texture::Set(size_t unit) {
  GL_CALL(glActiveTexture(GL_TEXTURE0 + unit));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, id_));
}

void Texture::Set(size_t unit) const { const_cast<Texture *>(this)->Set(unit); }

void Texture::Delete() {
  if (id_) {
    GL_CALL(glDeleteTextures(1, &id_));
    id_ = 0;
  }
}

void Material::Set(Renderer &renderer) {
  renderer.SetBlendMode(blend_mode_);
  for (size_t i = 0; i < textures_.size(); i++) textures_[i]->Set(i);
}

void Material::DeleteTextures() {
  for (size_t i = 0; i < textures_.size(); i++) textures_[i]->Delete();
}

}  // namespace fpl
