#include <memory>
#include "Bullet.h"
#include "dx11mathutil.h"
#include "CModel.h"
#include "CDirectInput.h"
#include "ModelMgr.h"

constexpr float BULLETSPEED = 5.0f;
constexpr uint32_t BULLETLIFE = 60;

bool Bullet::Init()
{
    bool sts = true;
    //�s�񏉊���
    DX11MtxIdentity(m_mtx);

    return sts;
}

void Bullet::Update()
{
    //�S�e�X�V
    for (auto& b : g_bullets) {
        b->m_life--;
        if (b->m_life > 0)
        {
            b->m_pos.x = b->m_pos.x + b->m_speed * b->m_direction.x;
            b->m_pos.y = b->m_pos.y + b->m_speed * b->m_direction.y;
            b->m_pos.z = b->m_pos.z + b->m_speed * b->m_direction.z;

            b->m_mtx._41 = b->m_pos.x;
            b->m_mtx._42 = b->m_pos.y;
            b->m_mtx._43 = b->m_pos.z;

        }
        else
        {
            b->m_sts = BULLETSTATUS::DEAD;
        }
    }
    //����ł���e�폜
    auto it = g_bullets.begin();
    while (it != g_bullets.end())
    {
        if ((*it)->isLive() == false)
        {
            it = g_bullets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Bullet::Draw()
{
 //���f���`��
        //�S�e�`��
    for (auto& b : g_bullets) {
        if (b->m_sts == BULLETSTATUS::LIVE)
        {
            b->m_pmodel->Draw(b->m_mtx);
        }
    }

}

void Bullet::Finalize()
{
}

void Bullet::FireBullet(DirectX::XMFLOAT4X4 mtx, DirectX::XMFLOAT3 ppos)
{
    //std::unique_ptr<Bullet> p;

    //p = std::make_unique<Bullet>();

    ////�e�̏�����
    //p->Init();

    ////���f���Z�b�g
    //p->SetModel(
    //    ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::MISSILE)].modelname)
    //);

    ////�����ʒu�Z�b�g
    //p->SetPos(ppos);
    //p->SetDirection(mtx);
    ////�Ō���ɒe��ǉ�
    //g_bullets.emplace_back(std::move(p));
}


