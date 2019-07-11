#pragma once
#include <simd/simd.h>
#import "Metal/Metal.h"
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

struct Parameters {
  vector_float2 resolution;
  vector_float3 right;
  vector_float3 up;
  vector_float3 forward;
  vector_float3 camera_position;
  uint32 triangle_count;
  uint32 max_hitpoint_count;
};

class MetalBackend final : public RenderingBackend {
 public:
  MetalBackend();

  void Prepare(const Scene& scene, size_t viewport_width,
               size_t viewport_height) override;
  void Render(const Camera& camera, Viewport* viewport) override;

 private:
  id<MTLDevice> device_;
  id<MTLLibrary> library_;
  id<MTLFunction> generate_hitpoints_function_;
  id<MTLFunction> evaluate_radiance_function_;
  id<MTLFunction> clear_texture_function_;
  id<MTLComputePipelineState> generate_hitpoints_pipeline_state_;
  id<MTLComputePipelineState> evaluate_radiance_pipeline_state_;
  id<MTLComputePipelineState> clear_texture_pipeline_state_;
  id<MTLTexture> output_texture_;
  id<MTLCommandQueue> command_queue_;
  id<MTLBuffer> parameters_buffer_;
  id<MTLBuffer> material_buffer_;
  id<MTLBuffer> vertex_buffer_;
  id<MTLBuffer> triangle_buffer_;
  id<MTLBuffer> hitpoint_counter_buffer_;
  id<MTLBuffer> hitpoint_buffer_;
  Parameters parameters_;
};

}  // namespace rainbow