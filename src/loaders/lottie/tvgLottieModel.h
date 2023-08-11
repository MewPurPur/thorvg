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

#ifndef _TVG_LOTTIE_MODEL_H_
#define _TVG_LOTTIE_MODEL_H_

#include "tvgCommon.h"
#include "tvgRender.h"
#include "tvgLottieProperty.h"


struct LottieComposition;

struct LottieStroke
{
    struct DashAttr
    {
        //0: offset, 1: dash, 2: gap
        LottieFloat value[3] = {0.0f, 0.0f, 0.0f};
    };

    virtual ~LottieStroke()
    {
        delete(dashattr);
    }

    LottieFloat& dash(int no)
    {
        if (!dashattr) dashattr = new DashAttr;
        return dashattr->value[no];
    }

    float dashOffset(int32_t frameNo)
    {
        return dash(0)(frameNo);
    }

    float dashGap(int32_t frameNo)
    {
        return dash(1)(frameNo) + dash(2)(frameNo);
    }

    float dashSize(int32_t frameNo)
    {
        return dash(1)(frameNo);
    }

    bool dynamic()
    {
        if (width.frames || dashattr) return true;
        return false;
    }

    LottieFloat width = 0.0f;
    DashAttr* dashattr = nullptr;
    float miterLimit = 0;
    StrokeCap cap = StrokeCap::Butt;
    StrokeJoin join = StrokeJoin::Miter;
};



struct LottieGradient
{
    bool dynamic()
    {
        if (start.frames || end.frames || opacity.frames || height.frames || angle.frames || colorStops.frames) return true;
        return false;
    }

    Fill* fill(int32_t frameNo);

    LottiePoint start = Point{0.0f, 0.0f};
    LottiePoint end = Point{0.0f, 0.0f};
    LottieOpacity opacity = 255;
    LottieFloat height = 0.0f;      //TODO:
    LottieFloat angle = 0.0f;
    LottieColorStop colorStops;
    uint8_t id = 0;    //1: linear, 2: radial
};


struct LottieObject
{
    enum Type : uint8_t
    {
        Composition = 0,
        Layer,
        Group,
        Transform,
        SolidFill,
        SolidStroke,
        GradientFill,
        GradientStroke,
        Rect,
        Ellipse,
        Path,
        Polystar,
        Trim,
        Repeater,
        RoundedCorner,
        Image
    };

    virtual ~LottieObject()
    {
        free(name);
    }

    char* name = nullptr;
    Type type;
    bool statical = true;      //no keyframes
    bool hidden = false;
};


struct LottieShape : LottieObject
{
    virtual ~LottieShape() {}
    bool direction;   //path direction (clock wise vs coutner clock wise)
};


struct LottieRoundedCorner : LottieObject
{
    void prepare()
    {
        LottieObject::type = LottieObject::RoundedCorner;
        if (radius.frames) statical = false;
    }

    LottieFloat radius = 0.0f;
};


struct LottiePath : LottieShape
{
    void prepare()
    {
        LottieObject::type = LottieObject::Path;
        if (pathset.frames) statical = false;
    }

    LottiePathSet pathset = PathSet{nullptr, nullptr, 0, 0};
};


struct LottieRect : LottieShape
{
    void prepare()
    {
        LottieObject::type = LottieObject::Rect;
        if (position.frames || size.frames || round.frames) statical = false;
    }

    float roundness(int32_t frameNo)
    {
        return roundedCorner ? roundedCorner->radius(frameNo) : round(frameNo);
    }

    LottieRoundedCorner* roundedCorner = nullptr;
    LottiePosition position = Point{0.0f, 0.0f};
    LottiePoint size = Point{0.0f, 0.0f};
    LottieFloat round = 0.0f;
};


struct LottiePolyStar : LottieShape
{
    enum Type : uint8_t {Star = 1, Polygon};

    void prepare()
    {
        LottieObject::type = LottieObject::Polystar;
        if (position.frames || innerRadius.frames || outerRadius.frames || innerRoundness.frames || outerRoundness.frames || rotation.frames || ptsCnt.frames) statical = false;
    }

    LottiePosition position = Point{0.0f, 0.0f};
    LottieFloat innerRadius = 0.0f;
    LottieFloat outerRadius = 0.0f;
    LottieFloat innerRoundness = 0.0f;
    LottieFloat outerRoundness = 0.0f;
    LottieFloat rotation = 0.0f;
    LottieFloat ptsCnt = 0.0f;
    Type type = Polygon;
};


struct LottieEllipse : LottieShape
{
    void prepare()
    {
        LottieObject::type = LottieObject::Ellipse;
        if (position.frames || size.frames) statical = false;
    }

    LottiePosition position = Point{0.0f, 0.0f};
    LottiePoint size = Point{0.0f, 0.0f};
};


struct LottieTransform : LottieObject
{
    struct SeparateCoord
    {
        LottieFloat x = 0.0f;
        LottieFloat y = 0.0f;
    };

    ~LottieTransform()
    {
        delete(coords);
    }

    void prepare()
    {
        LottieObject::type = LottieObject::Transform;
        if (position.frames || rotation.frames || scale.frames || anchor.frames || opacity.frames) statical = false;
        else if (coords && (coords->x.frames || coords->y.frames)) statical = false;
    }

    LottiePosition position = Point{0.0f, 0.0f};
    LottieFloat rotation = 0.0f;
    LottiePoint scale = Point{100.0f, 100.0f};
    LottiePoint anchor = Point{0.0f, 0.0f};
    LottieOpacity opacity = 255;

    //either a position or separate coordinates
    SeparateCoord* coords = nullptr;
};


struct LottieSolidStroke : LottieObject, LottieStroke
{
    void prepare()
    {
        LottieObject::type = LottieObject::SolidStroke;
        if (color.frames || opacity.frames || LottieStroke::dynamic()) statical = false;
    }

    LottieColor color = RGB24{255, 255, 255};
    LottieOpacity opacity = 255;
    bool disabled = false;    //TODO: can't replace with hidden?
};


struct LottieSolidFill : LottieObject
{
    void prepare()
    {
        LottieObject::type = LottieObject::SolidFill;
        if (color.frames || opacity.frames) statical = false;
    }

    LottieColor color = RGB24{255, 255, 255};
    LottieOpacity opacity = 255;
    FillRule rule = FillRule::Winding;
    bool disabled = false;   //TODO: can't replace with hidden?
};


struct LottieGradientFill : LottieObject, LottieGradient
{
    void prepare()
    {
        LottieObject::type = LottieObject::GradientFill;
        if (LottieGradient::dynamic()) statical = false;
    }

    FillRule rule = FillRule::Winding;
};


struct LottieGradientStroke : LottieObject, LottieStroke, LottieGradient
{
    void prepare()
    {
        LottieObject::type = LottieObject::GradientStroke;
        if (LottieStroke::dynamic() || LottieGradient::dynamic()) statical = false;
    }
};


struct LottieImage : LottieObject
{
    union {
        char* b64Data = nullptr;
        char* path;
    };
    char* mimeType = nullptr;
    uint32_t size = 0;

    ~LottieImage()
    {
        free(b64Data);
        free(mimeType);
    }

    void prepare()
    {
        LottieObject::type = LottieObject::Image;
    }
};


struct LottieGroup : LottieObject
{
    virtual ~LottieGroup()
    {
        for (auto p = children.data; p < children.end(); ++p) delete(*p);
        delete(transform);
    }

    void prepare(LottieObject::Type type = LottieObject::Group);

    virtual uint8_t opacity(int32_t frameNo)
    {
        if (children.empty()) return 0;
        return (transform ? transform->opacity(frameNo) : 255);
    }

    Scene* scene = nullptr;               //tvg render data

    Array<LottieObject*> children;
    LottieTransform* transform = nullptr;
};


struct LottieLayer : LottieGroup
{
    enum Type : uint8_t {Precomp = 0, Solid, Image, Null, Shape, Text};

    ~LottieLayer()
    {
        if (refId) {
            //No need to free assets children because the Composition owns them.
            children.clear();
            free(refId);
        }
    }

    void prepare()
    {
        LottieGroup::prepare(LottieObject::Layer);

        /* if layer is hidden, only useulf data is its transform matrix.
           so force it to be a Null Layer and release all resource. */
        if (hidden) {
            type = LottieLayer::Null;
            children.reset();
            return;
        }
    }

    uint8_t opacity(int32_t frameNo) override
    {
        //return zero if the visibility is false.
        if (frameNo < inFrame || frameNo > outFrame) return 0;
        if (type == Null) return 255;
        return LottieGroup::opacity(frameNo);
    }

    int32_t remap(int32_t frameNo);

    //Optimize: compact data??
    RGB24 color = {255, 255, 255};
    CompositeMethod matteType = CompositeMethod::None;
    BlendMethod blendMethod = BlendMethod::Normal;
    LottieLayer* parent = nullptr;
    LottieFloat timeRemap = 0.0f;
    LottieComposition* comp = nullptr;

    float timeStretch = 1.0f;
    uint32_t w, h;
    int32_t inFrame = 0;
    int32_t outFrame = 0;
    uint32_t startFrame = 0;
    char* refId = nullptr;      //pre-composition reference.
    int16_t pid = -1;           //id of the parent layer.
    int16_t id = -1;            //id of the current layer.

    //cached data
    struct {
        int32_t frameNo = -1;
        Matrix matrix;
        uint8_t opacity;
    } cache;

    Type type = Null;
    bool autoOrient = false;
    bool mask = false;
    bool roundedCorner = false;
};


struct LottieComposition
{
    ~LottieComposition();

    float duration() const
    {
        return frameDuration() / frameRate;  // in second
    }

    int32_t frameAtTime(float timeInSec) const
    {
        auto p = timeInSec / duration();
        if (p < 0.0f) p = 0.0f;
        else if (p > 1.0f) p = 1.0f;
        return (int32_t)lroundf(p * frameDuration());
    }

    uint32_t frameCnt() const
    {
        return frameDuration() + 1;
    }

    uint32_t frameDuration() const
    {
        return endFrame - startFrame;
    }

    Scene* scene = nullptr;       //tvg render data

    LottieLayer* root = nullptr;
    char* version = nullptr;
    char* name = nullptr;
    uint32_t w, h;
    int32_t startFrame, endFrame;
    float frameRate;
    Array<LottieObject*> assets;
    Array<LottieInterpolator*> interpolators;
};

#endif //_TVG_LOTTIE_MODEL_H_