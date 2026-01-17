#pragma once

#include <vector>
#include "Engine/Async.hpp"
#include "Engine/GL/Frame.hpp"
#include "Engine/GL/Program.h"
#include "Labs/Common/ICase.h"
#include "Labs/Common/ImageRGB.h"
#include "Labs/Common/ImGuiHelper.h"
#include "Labs/Final/Task1.h"
namespace VCX::Labs::Final {
    class CasePathTracing : public Common::ICase {
    public:
    // 场景是参数化的，不需要导入模型
        CasePathTracing();

        virtual std::string_view const GetName() override { return "Path Tracing"; }

        virtual void                     OnSetupPropsUI() override;
        virtual Common::CaseRenderResult OnRender(std::pair<std::uint32_t, std::uint32_t> const desiredSize) override;
        virtual void                     OnProcessInput(ImVec2 const & pos) override;

        // 进度条：设置渲染进度的回调函数
        using ProgressCallback = std::function<void(float)>; // 回调函数类型定义
        void SetProgressCallback(ProgressCallback callback); // 设置回调函数的方法
    private:
        std::array<Engine::GL::UniqueTexture2D, 2> _textures;
        std::array<Common::ImageRGB, 2>            _empty;
        Engine::Async<Common::ImageRGB>            _task;

        int  _sizeId     = 0; //渲染尺寸
        int  _bgId       = 0; //选择背景
        bool _enableZoom = true;
        bool _recompute  = true;
        int _samps = 1;  // 当前的采样数量，默认为 1

        //进度条
        float _progress = 0.0f; // 渲染进度 (0.0 - 1.0)
        std::atomic<bool> _isRendering = false; // 渲染状态标志
        ProgressCallback _progressCallback; // 渲染进度的回调函数
    };
}