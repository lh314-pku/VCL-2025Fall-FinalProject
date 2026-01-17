#include <algorithm>
#include <array>

// #include <std_image_write.h>

#include "Engine/loader.h"
#include "Labs/Common/ImGuiHelper.h"  
#include "Labs/Final/CaseTest.h"

namespace VCX::Labs::Final {
    // 用于侧边栏开发
    void CaseTest::OnSetupPropsUI() {
        // 还是把 UI 与 变量绑定，作为信号源
        ImGui::Checkbox("Test", &_enableTest);
    }
    // 用于主窗口开发
    Common::CaseRenderResult CaseTest::OnRender(std::pair<std::uint32_t, std::uint32_t> const desiredSize) {
        // 创建 400 x 400 的棋盘格: image
        Common::ImageRGB image = Common::CreateCheckboardImageRGB(400, 400);
        // 将其应用到 _texture[0]
        // 可以设置一系列纹理，用 std::vector 管理
        _texture.Update(image);
        return Common::CaseRenderResult {
            .Fixed = true,
            .Image = _texture,
            .ImageSize = {400, 400},
        };
    }
}