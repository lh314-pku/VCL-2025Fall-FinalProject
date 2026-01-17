#pragma once

#include "Engine/Async.hpp"
#include "Labs/Common/ICase.h"
#include "Labs/Common/ImageRGB.h"

namespace VCX::Labs::Final {
    class CaseTest : public Common::ICase {
    public:
        // 构造函数
        // CaseTest();
        // 虽然这部分代码可以直接 copy，但是读懂代码还是比较考验 cpp 功底的
        virtual std::string_view const GetName() override { return "Test"; }
        // 这里就是要求的三个函数：
        // 每个 Case 类都需要重载这三个函数
        virtual void                     OnSetupPropsUI() override;
        virtual Common::CaseRenderResult OnRender(std::pair<std::uint32_t, std::uint32_t> const desiredSize) override;
        // virtual void                     OnProcessInput(ImVec2 const & pos) override;
    private:
        Engine::GL::UniqueTexture2D _texture;
        
        bool _enableTest = true;
    };
}