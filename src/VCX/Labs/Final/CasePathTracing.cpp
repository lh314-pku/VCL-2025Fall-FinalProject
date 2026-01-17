#include "Engine/loader.h"
#include "Labs/Final/CasePathTracing.h"
#include "Labs/Common/ImageRGB.h"
#include "Labs/Common/ImGuiHelper.h"

#include <algorithm>
#include <array>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
namespace VCX::Labs::Final {
    static constexpr auto c_Sizes = std::to_array<std::pair<std::uint32_t, std::uint32_t>>({
        { 512U, 384U },
        { 768U, 576U } 
    });

    static constexpr auto c_SizeItems = std::array<char const *, 2> {
        "Small (512 x 384)",
        "Large (1024 x 768)"
    };

    static constexpr auto c_BgItems = std::array<char const *, 3> {
        "White",
        "Black",
        "Checkboard"
    };

    // 初始化可用采样数量的滑块范围
    static constexpr int c_SampsMin = 1;
    static constexpr int c_SampsMax = 100;

    CasePathTracing::CasePathTracing() :
        _textures(Engine::make_array<Engine::GL::UniqueTexture2D, c_Sizes.size()>(
            Engine::GL::SamplerOptions {
                .MinFilter = Engine::GL::FilterMode::Linear,
                .MagFilter = Engine::GL::FilterMode::Nearest
            })),
        _empty({
            Common::CreatePureImageRGB(c_Sizes[0].first, c_Sizes[0].second, { 2.f / 17, 2.f / 17, 2.f / 17 }),
            Common::CreatePureImageRGB(c_Sizes[1].first, c_Sizes[1].second, { 2.f / 17, 2.f / 17, 2.f / 17 })
        }) ,
         _samps(1), // 初始化采样值
        _recompute(true) // 默认需要重新计算
        {
    }

    //进度条：设置进度回调
    void CasePathTracing::SetProgressCallback(ProgressCallback callback){
        _progressCallback = std::move(callback);
    }

    //控制边栏
    void CasePathTracing::OnSetupPropsUI() {
        ImGui::Checkbox("Zoom Tooltip", &_enableZoom);
        //调整尺寸
        _recompute |= ImGui::Combo("Size", &_sizeId, c_SizeItems.data(), c_SizeItems.size());
        
        // 添加滑块用于调整采样值
        int prevSamps = _samps;
        ImGui::SliderInt("SPP/4", &_samps, c_SampsMin, c_SampsMax);
        if (prevSamps != _samps) {
            _recompute = true; // 如果采样值发生变化，则标记需要重新计算
        }

        // 添加渲染进度条
        if (_isRendering) {
            ImGui::Text("Rendering...");
            ImGui::ProgressBar(_progress, ImVec2(-1, 0));
        } else {
            ImGui::Text("Ready");
    }
    }
 
    //渲染
    Common::CaseRenderResult CasePathTracing::OnRender(std::pair<std::uint32_t, std::uint32_t> const desiredSize) {
        //auto const width = 512;
        //auto const height = 384;
        std::uint32_t width = c_Sizes[_sizeId].first;   
        std::uint32_t height = c_Sizes[_sizeId].second;

        if(_recompute){
            _recompute = false;
            _task.Emplace([this, width, height](){
                Common::ImageRGB image (width, height);
                std::cout << "OnRender lambda start" << std::endl;

                //进度条
                _isRendering = true;
                _progress = 0.0f;

                // std::cout << "Calling PathTrace" << std::endl;
                Vec* result = PathTrace(width, height, _samps, [this](float progress){
                    _progress = progress;
                });
                // std::cout << "PathTrace done" << std::endl; 

                for (std::size_t y = 0; y < height; ++y){
                    for (std::size_t x = 0; x < width; ++x){
                        int i = y * width + x;
                        image.At(x,y)={correct(result[i].x), correct(result[i].y), correct(result[i].z)};
                    }
                }//遍历pixels结束
                delete[] result;

                //进度条
                _isRendering = false;
                std::cout << "OnRender lambda done" << std::endl;

                return image;
            }
            );
        }
        
        //Common::ImageRGB image = Common::CreatePureImageRGB(width, height, { 0., 0., 0. });
        //Vec* c = PathTracing(512, 384, 1);        
    
        
        _textures[_sizeId].Update(_task.ValueOr(_empty[_sizeId]));
        return Common::CaseRenderResult {
            .Fixed     = true,
            .Image     = _textures[_sizeId],
            .ImageSize = {width,height},
        };
    }

    //交互
    void CasePathTracing::OnProcessInput(ImVec2 const & pos) {
        auto         window  = ImGui::GetCurrentWindow();
        bool         hovered = false;
        bool         anyHeld = false;
        ImVec2 const delta   = ImGui::GetIO().MouseDelta;
        ImGui::ButtonBehavior(window->Rect(), window->GetID("##io"), &hovered, &anyHeld);
        if (! hovered) return;
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && delta.x != 0.f)
            ImGui::SetScrollX(window, window->Scroll.x - delta.x);
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && delta.y != 0.f)
            ImGui::SetScrollY(window, window->Scroll.y - delta.y);
        if (_enableZoom && ! anyHeld && ImGui::IsItemHovered())
            Common::ImGuiHelper::ZoomTooltip(_textures[_sizeId], c_Sizes[_sizeId], pos);
    }
}