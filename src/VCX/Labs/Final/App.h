#pragma once

#include <vector>

#include "Engine/app.h"
#include "Labs/Common/UI.h"

#include "Labs/Final/CaseTest.h"
#include "Labs/Final/CasePathTracing.h"

namespace VCX::Labs::Final {
    class App : public Engine::IApp {
    private:
        Common::UI              _ui;
        std::size_t             _caseId = 0;
        CaseTest _caseTest;
        CasePathTracing _casePathTracing;

        std::vector<std::reference_wrapper<Common::ICase>> _cases = {
            _caseTest,
            _casePathTracing
        };

    public:
        App();

        void OnFrame() override;
    };
}
