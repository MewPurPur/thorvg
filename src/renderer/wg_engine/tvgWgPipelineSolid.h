/*
 * Copyright (c) 2023 the ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _TVG_WG_PIPELINE_SOLID_H_
#define _TVG_WG_PIPELINE_SOLID_H_

#include "tvgWgPipelineBase.h"

class WgPipelineSolid;

struct WgPipelineSolidColorInfo {
    float color[4]{};
};

struct WgPipelineDataSolid: WgPipelineData {
    WgPipelineSolidColorInfo uColorInfo{}; // @binding(1)

    void updateColor(const uint8_t* color);
};

class WgPipelineBindGroupSolid: public WgPipelineBindGroup {
private:
    WGPUBuffer uBufferColorInfo{}; // @binding(1)
public:
    void initialize(WGPUDevice device, WgPipelineSolid& pipelinePipelineSolid);
    void release();

    void update(WGPUQueue mQueue, WgPipelineDataSolid& pipelineDataSolid);
};

class WgPipelineSolid: public WgPipelineBase {
public:
    void initialize(WGPUDevice device) override;
    void release() override;
};

#endif //_TVG_WG_PIPELINE_SOLID_H_
