#include    "main.h"
#include    "Application.h"

// �G���g���[�|�C���g
int main(void)
{

#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
    HWND consoleWindow = GetConsoleWindow(); // �R���\�[���E�B���h�E�̃n���h�����擾
    ShowWindow(consoleWindow, SW_HIDE);     // �R���\�[���E�B���h�E���\���ɂ���
#endif//defined(DEBUG) || defined(_DEBUG)

    // �A�v���P�[�V�������s
    Application app(SCREEN_WIDTH,SCREEN_HEIGHT);
    app.Run();

    return 0;
}