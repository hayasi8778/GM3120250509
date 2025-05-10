#include	"system/renderer.h"
#include	"fpscontrol.h"

void gameinit() 
{
	// �����_���̏�����
	Renderer::Init();

}

void gameupdate(uint64_t deltatime)
{

}

void gamedraw(uint64_t deltatime) 
{
	// �����_�����O�O����
	Renderer::Begin();

	// �����_�����O�㏈��
	Renderer::End();

}

void gamedispose() 
{
	// �����_���̏I������
	Renderer::Uninit();

}

void gameloop()
{
	uint64_t delta_time = 0;

	// �t���[���̑҂����Ԃ��v�Z����
	static FPS fpsrate(70);

	// �O����s����Ă���̌o�ߎ��Ԃ��v��
	delta_time = fpsrate.CalcDelta();

	// �X�V�����A�`�揈�����Ăяo��
	gameupdate(delta_time);
	gamedraw(delta_time);

	// �K�莞�Ԃ܂�WAIT
	fpsrate.Wait();
}